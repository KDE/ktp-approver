add_definitions(-DTRANSLATION_DOMAIN=\"kded_ktp_approver\")

set(ktp_approver_SRCS
    ktp_approver_debug.cpp
    ktpapprovermodule.cpp
    approverdaemon.cpp
    dispatchoperation.cpp
    handlewithcaller.cpp
    channelapprover.cpp
    textchannelapprover.cpp
    tubechannelapprover.cpp
    filetransferchannelapprover.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(kded_ktp_approver MODULE ${ktp_approver_SRCS})
target_link_libraries(kded_ktp_approver Qt5::Widgets
                                        Qt5::Network
                                        TelepathyQt5::Core
                                        KF5::DBusAddons
                                        KF5::ConfigGui
                                        KF5::I18n
                                        KF5::Notifications
                                        KF5::Service
)

install(TARGETS kded_ktp_approver DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES ktp_approver.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR}/kded)
install(FILES ktp_approverrc DESTINATION ${KDE_INSTALL_CONFDIR})

find_file(DBUS_SEND_PATH dbus-send HINTS /usr/bin)
configure_file(org.freedesktop.Telepathy.Client.KTp.Approver.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.Approver.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.Approver.service
        DESTINATION ${KDE_INSTALL_DBUSSERVICEDIR})
