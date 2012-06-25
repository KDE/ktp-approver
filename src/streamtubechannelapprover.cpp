/*
    Copyright (C) 2012  David Edmundson <kde@davidedmundson.co.uk>

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
#include "streamtubechannelapprover.h"

#include <KNotification>
#include <KStatusNotifierItem>
#include <KLocale>
#include <KDebug>
#include <KAboutData>
#include <KActionCollection>
#include <KMenu>
#include <KIcon>

#include <TelepathyQt/AvatarData>
#include <TelepathyQt/Contact>

StreamTubeChannelApprover::StreamTubeChannelApprover(
        const Tp::StreamTubeChannelPtr & channel,
        QObject *parent)
    : ChannelApprover(0),  //set parent to 0. I.e don't delete when the dispatch operation finishes. Instead we delete when the channel becomes invalidated.

      m_channel(channel)
{
    kDebug();

    connect(m_channel.data(), SIGNAL(invalidated(Tp::DBusProxy*,QString,QString)), SLOT(onChannelInvalidated()));

    if (m_channel->service() == QLatin1String("x-ssh-contact")) {
        m_serviceName = i18nc("Acronym for the secure shell service","SSH");
    } else {
        m_serviceName = m_channel->service();
    }

    //notification
    m_notification = new KNotification("incoming_file_transfer");
    KAboutData aboutData("ktelepathy",0,KLocalizedString(),0);
    m_notification.data()->setComponentData(KComponentData(aboutData));
    m_notification.data()->setTitle(i18n("Incoming %1 share request", m_serviceName));

    Tp::ContactPtr sender = channel->initiatorContact();
    m_notification.data()->setText(i18n("<p>%1 wants to start a %2 sharing session with you</p>",
                                        sender->alias(),
                                        m_serviceName));

    QPixmap pixmap;
    if (pixmap.load(sender->avatarData().fileName)) {
        m_notification.data()->setPixmap(pixmap);
    }

    m_notification.data()->setActions(QStringList() << i18n("Accept") << i18n("Reject"));
    connect(m_notification.data(), SIGNAL(action1Activated()), SIGNAL(channelAccepted()));
    connect(m_notification.data(), SIGNAL(action2Activated()), SIGNAL(channelRejected()));

    m_notification.data()->sendEvent();

    //tray icon
    m_notifierItem = new KStatusNotifierItem;
    m_notifierItem->setCategory(KStatusNotifierItem::Communications);
    m_notifierItem->setStatus(KStatusNotifierItem::NeedsAttention);
    m_notifierItem->setIconByName(QLatin1String("utilities-terminal"));
    m_notifierItem->setStandardActionsEnabled(false);
    m_notifierItem->setTitle(i18n("Incoming share request"));
    m_notifierItem->setToolTip(QLatin1String("document-save"),
                               i18n("Incoming share request from %1", sender->alias()),
                               QString());
    m_notifierItem->contextMenu()->addAction(i18n("Accept"), this, SIGNAL(channelAccepted()));
    m_notifierItem->contextMenu()->addAction(i18n("Reject"), this, SIGNAL(channelRejected()));
//    connect(m_notifierItem, SIGNAL(activateRequested(bool,QPoint)), SIGNAL(channelAccepted())); //do we want clicking the approver to be auto-accept? I think not.
    connect(this, SIGNAL(channelAccepted()), SLOT(onChannelAccepted()));
}

StreamTubeChannelApprover::~StreamTubeChannelApprover()
{
    kDebug();

    //destroy the notification
    if (m_notification) {
        m_notification.data()->close();
    }

    //destroy the tray icon
    m_notifierItem->deleteLater();
}

void StreamTubeChannelApprover::onChannelAccepted()
{
    //destroy the notification
    if (m_notification) {
        m_notification.data()->close();
    }

    m_notifierItem->setTitle(i18n("%1 share with %2", m_serviceName, m_channel->initiatorContact()->alias()));
    //set new menu to an entry to close the channel
    m_notifierItem->contextMenu()->clear();
    m_notifierItem->contextMenu()->addAction(KIcon("dialog-close"), i18n("Stop %1 Sharing", m_serviceName), this, SLOT(onChannelCloseRequested()));
}

void StreamTubeChannelApprover::onChannelCloseRequested()
{
    m_channel->requestClose();
}

void StreamTubeChannelApprover::onChannelInvalidated()
{
    deleteLater();
}

#include "streamtubechannelapprover.moc"
