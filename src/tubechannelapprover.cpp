/*
    Copyright (C) 2012  David Edmundson <kde@davidedmundson.co.uk>
    Copyright (C) 2013  Dan Vrátil <dvratil@redhat.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tubechannelapprover.h"

#include <KService>
#include <KServiceTypeTrader>
#include <KStatusNotifierItem>
#include <KDebug>
#include <KAboutData>
#include <KIcon>
#include <KMenu>

#include <TelepathyQt/TubeChannel>
#include <TelepathyQt/StreamTubeChannel>
#include <TelepathyQt/DBusTubeChannel>
#include <TelepathyQt/Contact>

#include "approverdaemon.h"

TubeChannelApprover::TubeChannelApprover(const Tp::TubeChannelPtr& channel, QObject* parent):
    ChannelApprover(0),
    m_channel(channel)
{
    Q_UNUSED(parent);

    kDebug() << "Incoming tube channel";
    kDebug() << "\tTube Type:" << channel->channelType();

    connect(m_channel.data(), SIGNAL(invalidated(Tp::DBusProxy*,QString,QString)), SLOT(onChannelInvalidated()));

    QString serviceName;
    if (Tp::StreamTubeChannelPtr streamTube = Tp::StreamTubeChannelPtr::dynamicCast(channel)) {
        kDebug() << "\tService:" << streamTube->service();
        serviceName = streamTube->service();
    } else if (Tp::DBusTubeChannelPtr dbusTube = Tp::DBusTubeChannelPtr::dynamicCast(channel)) {
        kDebug() << "\tService name:" << dbusTube->serviceName();
        serviceName = dbusTube->serviceName();
    }

    KService::List services = KServiceTypeTrader::self()->query(QLatin1String("KTpApprover"));
    kDebug() << "Found" << services.count() << "KTpApprover services";
    if (!services.isEmpty()) {
        Q_FOREACH(const KService::Ptr &service, services) {
            if ((service->property(QLatin1String("X-KTp-ChannelType")) != channel->channelType()) ||
                (service->property(QLatin1String("X-KTp-Service")) != serviceName)) {

                continue;
            }
            m_service = service;
        }
    }


    if (!m_service) {
        kDebug() << "No service to match" << channel->channelType() << "," << serviceName;
        showNotification(i18n("Unknown Incoming Connection"),
                         i18n("%1 wants to start an unknown service with you", channel->initiatorContact()->alias()),
                         QLatin1String("dialog-warning"),
                         channel->initiatorContact());

        return;
    }

    showNotification(m_service->name(), m_service->comment(), m_service->icon(), channel->initiatorContact());
}

TubeChannelApprover::~TubeChannelApprover()
{
    kDebug();

    //destroy the notification
    if (m_notification) {
        m_notification.data()->close();
        m_notification.data()->deleteLater();
    }

    if (m_notifierItem) {
        m_notifierItem.data()->deleteLater();
    }
}

void TubeChannelApprover::showNotification(const QString& title, const QString& comment, const QString& icon, const Tp::ContactPtr& sender)
{
    // incoming_file_transfer = defines notification sound & user preferences
    m_notification = new KNotification(QLatin1String("incoming_file_transfer"), 0, KNotification::Persistent);
    KAboutData aboutData("ktelepathy", 0, KLocalizedString(), 0);
    m_notification.data()->setComponentData(KComponentData(aboutData));
    m_notification.data()->setTitle(title);
    if (comment.contains(QLatin1String("%1"))) {
        Q_ASSERT(sender);
        m_notification.data()->setText(comment.arg(sender->alias()));
    } else {
        m_notification.data()->setText(comment);
    }
    m_notification.data()->setPixmap(KIcon(icon).pixmap(32, 32));
    m_notification.data()->setActions(QStringList() << i18n("Accept") << i18n("Reject"));
    connect(m_notification.data(), SIGNAL(action1Activated()), SIGNAL(channelAccepted()));
    connect(m_notification.data(), SIGNAL(action2Activated()), SIGNAL(channelRejected()));

    m_notification.data()->sendEvent();

    //tray icon
    m_notifierItem = new KStatusNotifierItem;
    m_notifierItem.data()->setCategory(KStatusNotifierItem::Communications);
    m_notifierItem.data()->setStatus(KStatusNotifierItem::NeedsAttention);
    m_notifierItem.data()->setIconByName(icon);
    m_notifierItem.data()->setStandardActionsEnabled(false);
    m_notifierItem.data()->setTitle(title);
    m_notifierItem.data()->setToolTip(QLatin1String("document-save"),
                               i18n("Incoming %1 request from %2", title, sender->alias()),
                               QString());

    m_notifierItem.data()->contextMenu()->clear(); //calling clear removes the pointless title
    m_notifierItem.data()->contextMenu()->addAction(i18n("Accept"), this, SIGNAL(channelAccepted()));
    m_notifierItem.data()->contextMenu()->addAction(i18n("Reject"), this, SIGNAL(channelRejected()));
    connect(this, SIGNAL(channelAccepted()), SLOT(onChannelAccepted()));
}

void TubeChannelApprover::onChannelAccepted()
{
    //destroy the notification
    if (m_notification) {
        m_notification.data()->close();
        m_notification.data()->deleteLater();
    }

    m_notifierItem.data()->setStatus(KStatusNotifierItem::Active);
    if (!m_service.isNull() && m_service->property(QLatin1String("X-KTp-Cancellable")).toBool()) {
        m_notifierItem.data()->setTitle(i18n("%1 share with %2", m_service->name(), m_channel->initiatorContact()->alias()));
        //set new menu to an entry to close the channel
        m_notifierItem.data()->contextMenu()->clear();
        m_notifierItem.data()->contextMenu()->addAction(KIcon("dialog-close"), i18n("Stop %1 Sharing", m_service->name()),
                                                 this, SLOT(onChannelCloseRequested()));
    } else {
        m_notifierItem.data()->deleteLater();
    }
}

void TubeChannelApprover::onChannelCloseRequested()
{
    m_channel->requestClose();
}

void TubeChannelApprover::onChannelInvalidated()
{
    deleteLater();
}
