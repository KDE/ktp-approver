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
#ifndef STREAMTUBECHANNELAPPROVER_H
#define STREAMTUBECHANNELAPPROVER_H

#include "channelapprover.h"
#include <QtCore/QSharedPointer>
#include <TelepathyQt/StreamTubeChannel>

class KStatusNotifierItem;
class KNotification;

class StreamTubeChannelApprover : public ChannelApprover
{
    Q_OBJECT
public:
    StreamTubeChannelApprover(const Tp::StreamTubeChannelPtr & channel, QObject *parent);
    virtual ~StreamTubeChannelApprover();
private Q_SLOTS:
    void onChannelAccepted();
    void onChannelCloseRequested();
    void onChannelInvalidated();

private:
    QWeakPointer<KNotification> m_notification;
    KStatusNotifierItem *m_notifierItem;
    Tp::StreamTubeChannelPtr m_channel;
    QString m_serviceName;
};

#endif //STREAMTUBECHANNELAPPROVER_H
