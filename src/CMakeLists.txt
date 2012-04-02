set(ktp_approver_SRCS
    ktpapprovermodule.cpp
    approverdaemon.cpp
    dispatchoperation.cpp
    handlewithcaller.cpp
    channelapprover.cpp
    textchannelapprover.cpp
    filetransferchannelapprover.cpp
    streamtubechannelapprover.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

kde4_add_plugin(kded_ktp_approver ${ktp_approver_SRCS})
target_link_libraries(kded_ktp_approver ${KDE4_KDEUI_LIBS}
                                        ${TELEPATHY_QT4_LIBRARIES})

install(TARGETS kded_ktp_approver DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ktp_approver.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES ktp_approverrc DESTINATION ${CONFIG_INSTALL_DIR})

find_file(DBUS_SEND_PATH dbus-send HINTS /usr/bin)
configure_file(org.freedesktop.Telepathy.Client.KTp.Approver.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.Approver.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KTp.Approver.service
        DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
