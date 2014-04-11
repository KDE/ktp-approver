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
#include "dispatchoperation.h"
#include "channelapprover.h"
#include "handlewithcaller.h"
#include <KDebug>
#include <TelepathyQt/PendingOperation>

#include <TelepathyQt/TextChannel>
#include <TelepathyQt/ReceivedMessage>

DispatchOperation::DispatchOperation(const Tp::ChannelDispatchOperationPtr & dispatchOperation,
                                     QObject *parent)
    : QObject(parent), m_dispatchOperation(dispatchOperation)
{
    connect(m_dispatchOperation.data(), SIGNAL(channelLost(Tp::ChannelPtr,QString,QString)),
            SLOT(onChannelLost(Tp::ChannelPtr,QString,QString)));
    connect(m_dispatchOperation.data(), SIGNAL(invalidated(Tp::DBusProxy*,QString,QString)),
            SLOT(onDispatchOperationInvalidated(Tp::DBusProxy*,QString,QString)));

      m_channelApprover = ChannelApprover::create(dispatchOperation->channel(), this);
      Q_ASSERT(m_channelApprover);

      connect(m_channelApprover, SIGNAL(channelAccepted()), SLOT(onChannelAccepted()));
      connect(m_channelApprover, SIGNAL(channelRejected()), SLOT(onChannelRejected()));
}

DispatchOperation::~DispatchOperation()
{
    kDebug();
}

void DispatchOperation::onDispatchOperationInvalidated(Tp::DBusProxy *proxy,
                                                       const QString & errorName,
                                                       const QString & errorMessage)
{
    Q_UNUSED(proxy);
    kDebug() << "Dispatch operation invalidated" << errorName << errorMessage;
    deleteLater();
}

void DispatchOperation::onChannelAccepted()
{
    new HandleWithCaller(m_dispatchOperation, this);
}

void DispatchOperation::onChannelRejected()
{
    Tp::PendingOperation *operation = m_dispatchOperation->claim();
    connect(operation, SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onClaimFinished(Tp::PendingOperation*)));

}

void DispatchOperation::onClaimFinished(Tp::PendingOperation *operation)
{
    if (operation->isError()) {
        kDebug() << "Claim error:" << operation->errorName() << operation->errorMessage();
        return;
    }

    Tp::TextChannelPtr textChannel  = Tp::TextChannelPtr::dynamicCast(m_dispatchOperation->channel());
    if (textChannel) {
        //ack everything before we close the channel. Otherwise it will reappear
        textChannel->acknowledge(textChannel->messageQueue());
    }
    m_dispatchOperation->channel()->requestClose();
}

#include "dispatchoperation.moc"
