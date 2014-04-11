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
#include "channelapprover.h"

#include "textchannelapprover.h"
#include "filetransferchannelapprover.h"
#include "tubechannelapprover.h"

ChannelApprover *ChannelApprover::create(const Tp::ChannelPtr & channel, QObject *parent)
{
    if (channel->channelType() == TP_QT_IFACE_CHANNEL_TYPE_TEXT) {
        return new TextChannelApprover(Tp::TextChannelPtr::dynamicCast(channel), parent);
    }

    if (channel->channelType() == TP_QT_IFACE_CHANNEL_TYPE_FILE_TRANSFER1) {
        return new FileTransferChannelApprover(
                Tp::FileTransferChannelPtr::dynamicCast(channel),
                parent);
    }

    if ((channel->channelType() == TP_QT_IFACE_CHANNEL_TYPE_STREAM_TUBE1) ||
        (channel->channelType() == TP_QT_IFACE_CHANNEL_TYPE_DBUS_TUBE1)) {
        return new TubeChannelApprover(Tp::TubeChannelPtr::dynamicCast(channel), parent);
    }

    Q_ASSERT(false);
    return NULL;
}

#include "channelapprover.moc"
