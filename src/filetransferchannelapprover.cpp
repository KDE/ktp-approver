/*
    Copyright (C) 2010-2011 Collabora Ltd. <info@collabora.co.uk>
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#include "filetransferchannelapprover.h"
#include "ktp_approver_debug.h"
#include <QMenu>
#include <KNotification>
#include <KStatusNotifierItem>
#include <KLocalizedString>
#include <TelepathyQt/AvatarData>
#include <TelepathyQt/Contact>

FileTransferChannelApprover::FileTransferChannelApprover(
        const Tp::FileTransferChannelPtr & channel,
        QObject *parent)
    : ChannelApprover(parent)
{
    qCDebug(APPROVER);

    //notification
    m_notification = new KNotification("incoming_file_transfer", 0, KNotification::Persistent);
    m_notification.data()->setComponentName(QStringLiteral("ktelepathy"));
    m_notification.data()->setTitle(i18n("Incoming file transfer"));

    Tp::ContactPtr sender = channel->initiatorContact();
    m_notification.data()->setText(i18n("<p>%1 is sending you a file. "
                                        "Do you accept the file transfer?</p>"
                                        "<p><i>Filename:</i> %2</p>",
                                        sender->alias(),
                                        channel->fileName()));

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
    m_notifierItem->setIconByName(QLatin1String("document-save"));
    m_notifierItem->setAttentionIconByName(QLatin1String("mail-unread-new"));
    m_notifierItem->setStandardActionsEnabled(false);
    m_notifierItem->setTitle(i18n("Incoming file transfer"));
    m_notifierItem->setToolTip(QLatin1String("document-save"),
                               i18n("Incoming file transfer from %1", sender->alias()),
                               QString());

    m_notifierItem->contextMenu()->clear(); //calling clear removes the pointless title
    m_notifierItem->contextMenu()->addAction(i18n("Accept"), this, SIGNAL(channelAccepted()));
    m_notifierItem->contextMenu()->addAction(i18n("Reject"), this, SIGNAL(channelRejected()));
    connect(m_notifierItem, SIGNAL(activateRequested(bool,QPoint)), SIGNAL(channelAccepted()));
}

FileTransferChannelApprover::~FileTransferChannelApprover()
{
    qCDebug(APPROVER);

    //destroy the notification
    if (m_notification) {
        m_notification.data()->close();
        m_notification.data()->deleteLater();
    }

    //destroy the tray icon
    delete m_notifierItem;
}

#include "filetransferchannelapprover.moc"
