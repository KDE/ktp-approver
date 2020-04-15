project(ktp-approver)
include(FeatureSummary)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "04")
set (RELEASE_SERVICE_VERSION_MICRO "0")

set(KTP_APPROVER_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

cmake_policy(SET CMP0002 OLD)

find_package(ECM 1.3.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package (Qt5 REQUIRED COMPONENTS Widgets Network)
find_package (KF5 REQUIRED COMPONENTS DBusAddons Config I18n Notifications Service)

find_package(TelepathyQt5 0.9.8 REQUIRED)

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/version.h @ONLY)

add_subdirectory(src)

install(FILES ktp-approver.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
