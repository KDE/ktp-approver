/*
    Copyright (C) 2010 Collabora Ltd. <info@collabora.co.uk>
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
#include "textchannelapprover.h"
#include <QMenu>
#include <KNotification>
#include <KStatusNotifierItem>
#include <KLocalizedString>

#include <TelepathyQt/ReceivedMessage>
#include <TelepathyQt/AvatarData>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingContacts>

TextChannelApprover::TextChannelApprover(const Tp::TextChannelPtr & channel, QObject *parent)
    : ChannelApprover(parent), m_notifierItem(getNotifierItem())
{
    Q_FOREACH(const Tp::ReceivedMessage & msg, channel->messageQueue()) {
        onMessageReceived(msg);
    }

    connect(channel.data(), SIGNAL(messageReceived(Tp::ReceivedMessage)),
            SLOT(onMessageReceived(Tp::ReceivedMessage)));
    connect(m_notifierItem.data(), SIGNAL(activateRequested(bool,QPoint)),
            SIGNAL(channelAccepted()));

    updateNotifierItemTooltip();
}

TextChannelApprover::~TextChannelApprover()
{
    //destroy the notification
    if (m_notification) {
        m_notification.data()->close();
        m_notification.data()->deleteLater();
    }
}

void TextChannelApprover::onMessageReceived(const Tp::ReceivedMessage & msg)
{
    if (msg.isDeliveryReport()) {
        return;
    }
    if (msg.deliveryDetails().status() == Tp::DeliveryStatusRead) {
        // The message is already read; nothing to notify about.
        return;
    }
    if (msg.isSilent()) {
        return;
    }

    if (!m_notification) {
        m_notification = new KNotification("new_text_message", 0, KNotification::Persistent);
        m_notification.data()->setComponentName(QStringLiteral("ktelepathy"));

        Tp::ContactPtr sender = msg.sender();
        if (sender) {
            m_notification.data()->setTitle(sender->alias());

            QPixmap pixmap;
            if (pixmap.load(sender->avatarData().fileName)) {
                m_notification.data()->setPixmap(pixmap);
            }
        } else {
            m_notification.data()->setTitle(i18n("Incoming message"));
        }

        m_notification.data()->setActions(QStringList() << i18n("Respond"));
        connect(m_notification.data(), SIGNAL(action1Activated()), SIGNAL(channelAccepted()));
    }

    m_notifierItem.data()->contextMenu()->clear(); //calling clear removes the pointless title
    m_notifierItem.data()->contextMenu()->addAction(i18n("Accept"), this, SIGNAL(channelAccepted()));
    m_notifierItem.data()->contextMenu()->addAction(i18n("Close"), this, SIGNAL(channelRejected()));

    m_notification.data()->setText(msg.text().simplified());
    m_notification.data()->sendEvent();
}

Q_GLOBAL_STATIC(QWeakPointer<KStatusNotifierItem>, s_notifierItem)

//static
QSharedPointer<KStatusNotifierItem> TextChannelApprover::getNotifierItem()
{
    QSharedPointer<KStatusNotifierItem> notifierItem = s_notifierItem->toStrongRef();
    if (!notifierItem) {
        notifierItem = QSharedPointer<KStatusNotifierItem>(new KStatusNotifierItem);
        notifierItem->setCategory(KStatusNotifierItem::Communications);
        notifierItem->setStatus(KStatusNotifierItem::NeedsAttention);
        notifierItem->setIconByName(QLatin1String("mail-unread"));
        notifierItem->setAttentionIconByName(QLatin1String("mail-unread-new"));
        notifierItem->setStandardActionsEnabled(false);
        notifierItem->setProperty("approver_new_channels_count", 0U);

        *s_notifierItem = notifierItem;
    }

    return notifierItem;
}

void TextChannelApprover::updateNotifierItemTooltip()
{
    QVariant channelsCount = m_notifierItem->property("approver_new_channels_count");
    channelsCount = QVariant(channelsCount.toUInt() + 1);
    m_notifierItem->setProperty("approver_new_channels_count", channelsCount);

    m_notifierItem->setToolTip(QLatin1String("mail-unread-new"),
                               i18np("You have 1 incoming conversation",
                                     "You have %1 incoming conversations",
                                     channelsCount.toUInt()),
                               QString());
}

#include "textchannelapprover.moc"
