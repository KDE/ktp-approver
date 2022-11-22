cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "22")
set (RELEASE_SERVICE_VERSION_MINOR "11")
set (RELEASE_SERVICE_VERSION_MICRO "90")

set(KTP_APPROVER_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(ktp-approver VERSION ${KTP_APPROVER_VERSION})

find_package(ECM 1.6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package (Qt5 REQUIRED COMPONENTS Widgets Network)
find_package (KF5 REQUIRED COMPONENTS DBusAddons Config I18n Notifications Service)

find_package(TelepathyQt5 0.9.8 REQUIRED)

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/version.h @ONLY)

add_subdirectory(src)

install(FILES ktp-approver.desktop DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR})

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
