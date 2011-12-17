set(telepathy_kde_approver_SRCS
    tpkdeapprovermodule.cpp
    approverdaemon.cpp
    dispatchoperation.cpp
    handlewithcaller.cpp
    channelapprover.cpp
    textchannelapprover.cpp
    filetransferchannelapprover.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

kde4_add_plugin(kded_telepathy_kde_approver ${telepathy_kde_approver_SRCS})
target_link_libraries(kded_telepathy_kde_approver ${KDE4_KDEUI_LIBS}
                                                  ${TELEPATHY_QT4_LIBRARIES})

install(TARGETS kded_telepathy_kde_approver DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES telepathy_kde_approver.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
install(FILES telepathy_kde_approverrc DESTINATION ${CONFIG_INSTALL_DIR})

find_file(DBUS_SEND_PATH dbus-send HINTS /usr/bin)
configure_file(org.freedesktop.Telepathy.Client.KDE.Approver.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KDE.Approver.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.freedesktop.Telepathy.Client.KDE.Approver.service
        DESTINATION ${DBUS_SERVICES_INSTALL_DIR})
