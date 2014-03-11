project(ktp-approver)

set(KTP_APPROVER_VERSION "0.8.0")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
cmake_policy(SET CMP0002 OLD)

find_package(KDE4 REQUIRED)
find_package(TelepathyQt4 0.8.9 REQUIRED)

include_directories(${KDE4_INCLUDES} ${TELEPATHY_QT4_INCLUDE_DIR})
add_definitions(${KDE4_DEFINITIONS})

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/version.h @ONLY)

add_subdirectory(src)

install(FILES ktp-approver.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
