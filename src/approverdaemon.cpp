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
#include "approverdaemon.h"
#include "dispatchoperation.h"
#include <TelepathyQt4/ChannelClassSpec>
#include <TelepathyQt4/ChannelDispatchOperation>

static inline Tp::ChannelClassSpecList channelClassSpecList()
{
    return Tp::ChannelClassSpecList()
                << Tp::ChannelClassSpec::textChat()
#if 0
                << Tp::ChannelClassSpec::unnamedTextChat()
                << Tp::ChannelClassSpec::textChatroom()
                << Tp::ChannelClassSpec::streamedMediaCall()
                << Tp::ChannelClassSpec::streamedMediaAudioCall()
                << Tp::ChannelClassSpec::streamedMediaVideoCall()
                << Tp::ChannelClassSpec::incomingFileTransfer()
#endif
                ;
}

ApproverDaemon::ApproverDaemon(QObject *parent)
    : QObject(parent), AbstractClientApprover(channelClassSpecList())
{
}

void ApproverDaemon::addDispatchOperation(const Tp::MethodInvocationContextPtr<> & context,
                                          const Tp::ChannelDispatchOperationPtr & dispatchOperation)
{
    new DispatchOperation(dispatchOperation, this);
    context->setFinished();
}

#include "approverdaemon.moc"
