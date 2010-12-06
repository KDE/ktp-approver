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
#include <KNotification>
#include <KStatusNotifierItem>
#include <KLocale>
#include <KGlobal>
#include <KDebug>
#include <TelepathyQt4/ReceivedMessage>
#include <TelepathyQt4/AvatarData>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingContacts>

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
}

TextChannelApprover::~TextChannelApprover()
{
    //destroy the notifications
    Q_FOREACH(KNotification *notification, m_notifications) {
        notification->close();
    }
    m_notifications.clear();
}

void TextChannelApprover::onMessageReceived(const Tp::ReceivedMessage & msg)
{
    Tp::ContactPtr sender = msg.sender();
    if (sender && (!sender->actualFeatures().contains(Tp::Contact::FeatureAlias) ||
                   !sender->actualFeatures().contains(Tp::Contact::FeatureAvatarData)))
    {
        new MessageReceivedContactUpgrader(msg, this);
        return;
    }

    KNotification *notification = new KNotification("new_text_message");
    notification->setText(msg.text());

    if (sender) {
        notification->setTitle(i18n("Incoming message from %1", sender->alias()));

        QPixmap pixmap;
        if (pixmap.load(sender->avatarData().fileName)) {
            notification->setPixmap(pixmap);
        }
    } else {
        notification->setTitle(i18n("Incoming message"));
    }

    notification->setActions(QStringList() << i18n("Respond"));
    connect(notification, SIGNAL(activated()), SIGNAL(channelAccepted()));

    notification->sendEvent();

    m_notifications.insert(notification);
    connect(notification, SIGNAL(destroyed(QObject*)), SLOT(onNotificationDestroyed(QObject*)));

    updateNotifierItemTooltip();
}

void TextChannelApprover::onNotificationDestroyed(QObject *notification)
{
    m_notifications.remove(reinterpret_cast<KNotification*>(notification));
}


K_GLOBAL_STATIC(QWeakPointer<KStatusNotifierItem>, s_notifierItem)

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
        notifierItem->setProperty("approver_new_messages_count", 0U);
        *s_notifierItem = notifierItem;
    }

    return notifierItem;
}

void TextChannelApprover::updateNotifierItemTooltip()
{
    QVariant numMessages = m_notifierItem->property("approver_new_messages_count");
    numMessages = QVariant(numMessages.toUInt() + 1);
    m_notifierItem->setProperty("approver_new_messages_count", numMessages);

    m_notifierItem->setToolTip(QLatin1String("mail-unread-new"),
                               i18n("You have %1 new unread messages", numMessages.toUInt()),
                               QString());
}


MessageReceivedContactUpgrader::MessageReceivedContactUpgrader(const Tp::ReceivedMessage & msg,
                                                               TextChannelApprover *parent)
    : QObject(parent), m_msg(msg), m_parent(parent)
{
    Tp::PendingContacts *pc = msg.sender()->manager()->upgradeContacts(
        QList<Tp::ContactPtr>() << msg.sender(),
        Tp::Features() << Tp::Contact::FeatureAlias << Tp::Contact::FeatureAvatarData
    );

    connect(pc, SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onUpgradeContactsFinished(Tp::PendingOperation*)));
}

void MessageReceivedContactUpgrader::onUpgradeContactsFinished(Tp::PendingOperation *operation)
{
    if (operation->isError()) {
        kError() << "Could not upgrade contact" << operation->errorName()
                                                << operation->errorMessage();
    } else {
        m_parent->onMessageReceived(m_msg);
    }
    deleteLater();
}

#include "textchannelapprover.moc"
