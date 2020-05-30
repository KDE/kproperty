set(KPROPERTY_GENERATE_PRI TRUE)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(REQUIRED_KF5_VERSION 5.36.0)
find_package(ECM ${REQUIRED_KF5_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
include(SetKPropertyCMakePolicies NO_POLICY_SCOPE)

project(KProperty VERSION 3.2.90) # Update this: Alpha = 90, Beta 1 = 91, RC 1 = 94

# CMake
include(CMakePackageConfigHelpers)
include(GenerateExportHeader)

# ECM
if(KPROPERTY_GENERATE_PRI)
    include(ECMGeneratePriFile)
endif()
include(ECMGenerateHeaders)
include(ECMOptionalAddSubdirectory)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECMakeSettings NO_POLICY_SCOPE)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddQch)

# Own
include(KPropertyAddIconsRccFile)
include(KPropertyMacros)

simple_option(BUILD_QCH "Build API documentation in QCH format" OFF)
simple_option(KPROPERTY_WIDGETS "Build the QtWidget components of KProperty" ON)
simple_option(KPROPERTY_KF "Build components depending on KDE Frameworks" ON)

# Dependencies
set(REQUIRED_QT_VERSION 5.4.0)

find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core)

get_git_revision_and_branch()

if(KPROPERTY_WIDGETS)
    find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Widgets)
    if(KPROPERTY_KF)
        find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED Config CoreAddons WidgetsAddons GuiAddons)
        #TODO KIO
    endif()
    add_tests()    #TODO add tests for KPropertyCore, also working when KPROPERTY_WIDGETS is OFF
    add_examples() #TODO add example for KPropertyCore, also working when KPROPERTY_WIDGETS is OFF
endif()

add_unfinished_features_option()
add_pc_file(KPropertyCore)
add_pc_file(KPropertyWidgets)

add_subdirectory(src)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ecm_install_po_files_as_qm(po)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
