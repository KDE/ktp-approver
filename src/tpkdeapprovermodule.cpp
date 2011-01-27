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
#include "tpkdeapproverfactory.h"

#include <KAboutData>
#include <KLocale>
#include <KComponentData>
#include <KDEDModule>

#include <TelepathyQt4/Types>
#include <TelepathyQt4/Debug>
#include <TelepathyQt4/ClientRegistrar>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/TextChannel>

class TpKDEApproverModule : public KDEDModule
{
public:
    TpKDEApproverModule(QObject *parent, const QVariantList & args)
        : KDEDModule(parent)
    {
        Q_UNUSED(args);

        Tp::registerTypes();
        Tp::enableDebug(true);
        Tp::enableWarnings(true);

        Tp::AccountFactoryPtr accountFactory =
            Tp::AccountFactory::create(QDBusConnection::sessionBus());
        Tp::ConnectionFactoryPtr connectionFactory =
            Tp::ConnectionFactory::create(QDBusConnection::sessionBus());

        Tp::ChannelFactoryPtr channelFactory =
            Tp::ChannelFactory::create(QDBusConnection::sessionBus());
        channelFactory->addCommonFeatures(Tp::Channel::FeatureCore);
        channelFactory->addFeaturesForTextChats(Tp::Features()
                                                    << Tp::TextChannel::FeatureCore
                                                    << Tp::TextChannel::FeatureMessageQueue);

        Tp::ContactFactoryPtr contactFactory =
            Tp::ContactFactory::create(Tp::Features()
                                        << Tp::Contact::FeatureAlias
                                        << Tp::Contact::FeatureAvatarData);

        m_registrar = Tp::ClientRegistrar::create(accountFactory, connectionFactory,
                                                  channelFactory, contactFactory);
        m_registrar->registerClient(Tp::SharedPtr<ApproverDaemon>(new ApproverDaemon()),
                                    "telepathy_kde_approver");
    }

    static inline KAboutData aboutData()
    {
        KAboutData aboutData("telepathy_kde_approver", 0, KLocalizedString(), "0.1",
                             KLocalizedString(), KAboutData::License_LGPL,
                             ki18n("(C) 2010, Collabora Ltd."));
        aboutData.addAuthor(ki18nc("@info:credit", "George Kiagiadakis"),
                            KLocalizedString(), "george.kiagiadakis@collabora.co.uk");
        return aboutData;
    }

private:
    Tp::ClientRegistrarPtr m_registrar;
};

K_PLUGIN_FACTORY_DEFINITION(TpKDEApproverFactory, registerPlugin<TpKDEApproverModule>();)
K_EXPORT_PLUGIN(TpKDEApproverFactory(TpKDEApproverModule::aboutData()))
