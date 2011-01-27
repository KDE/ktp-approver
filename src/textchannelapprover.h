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
#ifndef TEXTCHANNELAPPROVER_H
#define TEXTCHANNELAPPROVER_H

#include "channelapprover.h"
#include <QtCore/QSet>
#include <QtCore/QSharedPointer>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/ReceivedMessage>

class KStatusNotifierItem;
class KNotification;

class TextChannelApprover : public ChannelApprover
{
    Q_OBJECT
public:
    TextChannelApprover(const Tp::TextChannelPtr & channel, QObject *parent);
    virtual ~TextChannelApprover();

private Q_SLOTS:
    void onMessageReceived(const Tp::ReceivedMessage & msg);

private:
    static QSharedPointer<KStatusNotifierItem> getNotifierItem();
    void updateNotifierItemTooltip();

    QWeakPointer<KNotification> m_notification;
    QSharedPointer<KStatusNotifierItem> m_notifierItem;

    friend class MessageReceivedContactUpgrader;
};


/* This code is a temporary solution until tp-qt4 implements contact factories */
class MessageReceivedContactUpgrader : public QObject
{
    Q_OBJECT
public:
    explicit MessageReceivedContactUpgrader(const Tp::ReceivedMessage & msg,
                                            TextChannelApprover *parent);

private Q_SLOTS:
    void onUpgradeContactsFinished(Tp::PendingOperation *operation);

private:
    Tp::ReceivedMessage m_msg;
    TextChannelApprover *m_parent;
};

#endif //TEXTCHANNELAPPROVER_H
