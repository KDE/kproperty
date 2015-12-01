if(EXISTS "/usr/include/sailfishapp/sailfishapp.h") #SailfishOS
    cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
else()
    cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
endif()


project(KProperty)

# ECM setup
find_package(ECM 1.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

if(POLICY CMP0063) # Honor visibility properties for all target types (since cmake 3.3)
    cmake_policy(SET CMP0063 OLD)
endif()

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(ECMOptionalAddSubdirectory)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

include(FeatureSummary)
include(GenerateExportHeader)
include(MacroLogFeature)
include(KPropertyMacros)

simple_option(KPROPERTY_WIDGETS "Build the QtWidget components of KProperty" ON)

set(KPROPERTY_VERSION "2.97.0") # Update this

# Dependencies
set(REQUIRED_QT_VERSION "5.2.0")

find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core)

if(KPROPERTY_WIDGETS)
    find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Widgets)
    find_package(KF5 5.7.0 REQUIRED Config CoreAddons WidgetsAddons GuiAddons)

    #TODO KIO
    get_git_revision_and_branch()
    add_tests()
    add_examples()
endif()

add_unfinished_features_option()
add_pc_file(KPropertyCore)
add_pc_file(KPropertyWidgets)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
macro_display_feature_log()
