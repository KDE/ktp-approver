/*
    Copyright (C) 2012  David Edmundson <kde@davidedmundson.co.uk>
    Copyright (C) 2013  Dan Vrátil <dvratil@redhat.com>

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
#ifndef TUBECHANNELAPPROVER_H
#define TUBECHANNELAPPROVER_H

#include "channelapprover.h"

#include <KNotification>
#include <KService>
#include <TelepathyQt/TubeChannel>

class KStatusNotifierItem;

class TubeChannelApprover : public ChannelApprover
{
    Q_OBJECT

  public:
    TubeChannelApprover(const Tp::TubeChannelPtr &channel, QObject* parent);
    virtual ~TubeChannelApprover();

  private Q_SLOTS:
    void onChannelAccepted();
    void onChannelCloseRequested();
    void onChannelInvalidated();

  private:
    void showNotification(const QString &title, const QString &comment, const QString &icon,
                          const Tp::ContactPtr &sender);

    Tp::TubeChannelPtr m_channel;
    QWeakPointer<KNotification> m_notification;
    QWeakPointer<KStatusNotifierItem> m_notifierItem;
    KService::Ptr m_service;
};

#endif // TUBECHANNELAPPROVER_H
