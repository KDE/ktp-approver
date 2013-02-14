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
#include "ktpapproverfactory.h"

#include <KAboutData>
#include <KLocale>
#include <KComponentData>
#include <KDEDModule>

#include <TelepathyQt/Types>
#include <TelepathyQt/Debug>
#include <TelepathyQt/ClientRegistrar>
#include <TelepathyQt/Channel>
#include <TelepathyQt/TextChannel>
#include <TelepathyQt/IncomingFileTransferChannel>
#include <TelepathyQt/IncomingStreamTubeChannel>


class KTpApproverModule : public KDEDModule
{
public:
    KTpApproverModule(QObject *parent, const QVariantList & args)
        : KDEDModule(parent)
    {
        Q_UNUSED(args);

        Tp::registerTypes();
        Tp::enableDebug(false);
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
        channelFactory->addFeaturesForIncomingFileTransfers(
                Tp::IncomingFileTransferChannel::FeatureCore);

        channelFactory->addFeaturesForIncomingStreamTubes(
                    Tp::IncomingStreamTubeChannel::FeatureCore);

        Tp::ContactFactoryPtr contactFactory =
            Tp::ContactFactory::create(Tp::Features()
                                        << Tp::Contact::FeatureAlias
                                        << Tp::Contact::FeatureAvatarData);

        m_registrar = Tp::ClientRegistrar::create(accountFactory, connectionFactory,
                                                  channelFactory, contactFactory);
        m_registrar->registerClient(Tp::SharedPtr<ApproverDaemon>(new ApproverDaemon()),
                                    "KTp.Approver");
    }

    static inline KAboutData aboutData()
    {
        KAboutData aboutData("ktp-approver", "kded_ktp_approver", KLocalizedString(), "0.5.3",
                             KLocalizedString(), KAboutData::License_LGPL,
                             ki18nc("@info:credit", "(C) 2010, Collabora Ltd."));
        aboutData.addAuthor(ki18nc("@info:credit", "George Kiagiadakis"),
                            KLocalizedString(), "george.kiagiadakis@collabora.co.uk");
        return aboutData;
    }

private:
    Tp::ClientRegistrarPtr m_registrar;
};

K_PLUGIN_FACTORY_DEFINITION(KTpApproverFactory, registerPlugin<KTpApproverModule>();)
K_EXPORT_PLUGIN(KTpApproverFactory(KTpApproverModule::aboutData()))
