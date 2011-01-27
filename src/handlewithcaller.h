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
#ifndef HANDLEWITHCALLER_H
#define HANDLEWITHCALLER_H

#include <QtCore/QObject>
#include <TelepathyQt4/ChannelDispatchOperation>

class HandleWithCaller : public QObject
{
    Q_OBJECT
public:
    HandleWithCaller(const Tp::ChannelDispatchOperationPtr & dispatchOperation, QObject *parent);

private Q_SLOTS:
    void onHandleWithFinished(Tp::PendingOperation *operation);

private:
    void findHandlers();
    void callHandleWith();

    Tp::ChannelDispatchOperationPtr m_dispatchOperation;
    QStringList m_possibleHandlers;
};

#endif //HANDLEWITHCALLER_H
