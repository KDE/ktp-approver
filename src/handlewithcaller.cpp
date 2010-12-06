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
#include "handlewithcaller.h"
#include <TelepathyQt4/PendingOperation>

HandleWithCaller::HandleWithCaller(const Tp::ChannelDispatchOperationPtr & dispatchOperation,
                                   QObject *parent)
    : QObject(parent)
{
    m_dispatchOperation = dispatchOperation;
    m_possibleHandlers = dispatchOperation->possibleHandlers();
    Q_ASSERT(!m_possibleHandlers.isEmpty());

    callHandleWith();
}

void HandleWithCaller::callHandleWith()
{
    Tp::PendingOperation *op = m_dispatchOperation->handleWith(m_possibleHandlers.first());
    connect(op, SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onHandleWithFinished(Tp::PendingOperation*)));
}

void HandleWithCaller::onHandleWithFinished(Tp::PendingOperation* operation)
{
    if (operation->isError()) {
        if (operation->errorName() == TP_QT4_ERROR_INVALID_ARGUMENT ||
            operation->errorName() == TP_QT4_ERROR_NOT_AVAILABLE ||
            operation->errorName() == TP_QT4_ERROR_NOT_IMPLEMENTED)
        {
            //remove the handler that we just tried and try the next one
            m_possibleHandlers.removeFirst();
            if (!m_possibleHandlers.isEmpty()) {
                callHandleWith();
                return;
            }
        }
    }

    deleteLater();
}

#include "handlewithcaller.moc"
