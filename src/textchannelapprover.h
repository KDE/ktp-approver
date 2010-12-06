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
    void onNotificationDestroyed(QObject *notification);

private:
    static QSharedPointer<KStatusNotifierItem> getNotifierItem();
    void updateNotifierItemTooltip();

    QSet<KNotification*> m_notifications;
    QSharedPointer<KStatusNotifierItem> m_notifierItem;
};

#endif //TEXTCHANNELAPPROVER_H
