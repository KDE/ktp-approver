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
#include "ktpapproverfactory.h"
#include <KSharedConfig>
#include <KConfigGroup>
#include <KDebug>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/Channel>

HandleWithCaller::HandleWithCaller(const Tp::ChannelDispatchOperationPtr & dispatchOperation,
                                   QObject *parent)
    : QObject(parent)
{
    m_dispatchOperation = dispatchOperation;

    findHandlers();
    Q_ASSERT(!m_possibleHandlers.isEmpty());

    callHandleWith();
}

void HandleWithCaller::findHandlers()
{
    KSharedConfigPtr config = KSharedConfig::openConfig(KTpApproverFactory::componentData());
    KConfigGroup group = config->group("HandlerPreferences");

    //realistically, dispatch operations only have one channel
    //FIXME if there ever exists a case where a dispatch operation has more
    //than one channels, fix this code to do something more appropriate
    QString channelType = m_dispatchOperation->channel()->channelType();

    QStringList preferredHandlers = group.readEntry(channelType, QStringList());
    QStringList possibleHandlers = m_dispatchOperation->possibleHandlers();

    kDebug() << "Preferred:" << preferredHandlers;
    kDebug() << "Possible:" << possibleHandlers;

    //intersect the two lists, while respecting the order
    //of preference that was read from the config file
    QStringList::iterator it = preferredHandlers.begin();
    while(it != preferredHandlers.end()) {
        int index = possibleHandlers.indexOf(*it);
        if (index != -1) {
            possibleHandlers.removeAt(index);
            ++it;
        } else {
            it = preferredHandlers.erase(it);
        }
    }

    //preferredHandlers now contains the intersection. Because the intersection
    //can lead to an empty list, we append the rest of the possible handlers
    //to make sure it is not going to be empty.
    preferredHandlers.append(possibleHandlers);

    m_possibleHandlers = preferredHandlers;
    kDebug() << "Final:" << m_possibleHandlers;
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
        if (operation->errorName() == TP_QT_ERROR_INVALID_ARGUMENT ||
            operation->errorName() == TP_QT_ERROR_NOT_AVAILABLE ||
            operation->errorName() == TP_QT_ERROR_NOT_IMPLEMENTED)
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
