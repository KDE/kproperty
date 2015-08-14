cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(KProperty)

# ECM setup
find_package(ECM 1.8.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

if(POLICY CMP0063) # Honor visibility properties for all target types (since cmake 3.3)
    cmake_policy(SET CMP0063 OLD)
endif()

include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMPoQmTools)
include(ECMSetupVersion)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

include(FeatureSummary)
include(GenerateExportHeader)

set(KPROPERTY_VERSION "2.96.0") # Update this

# Dependencies
set(REQUIRED_QT_VERSION "5.2.0")

# Required components to build this framework
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Core Widgets)
find_package(KF5 5.7.0 REQUIRED Config CoreAddons WidgetsAddons GuiAddons)
#TODO KIO

macro(add_simple_feature_info _NAME _DESCRIPTION _DEFAULT)
  if ("${_DEFAULT}" STREQUAL "${${_NAME}}")
    set(_STATUS " (default value)")
  else ()
    set(_STATUS "")
  endif ()
  add_feature_info(${_NAME} ${_NAME} ${_DESCRIPTION}${_STATUS})
endmacro(add_simple_feature_info)

macro(simple_option _NAME _DESCRIPTION _DEFAULT)
  option(${_NAME} ${_DESCRIPTION} ${_DEFAULT})
  add_simple_feature_info(${_NAME} ${_DESCRIPTION} ${_DEFAULT})
endmacro(simple_option)

# If Git is installed and a '.git' directory is found set KPROPERTY_GIT_SHA1_STRING
set(KPROPERTY_GIT_SHA1_STRING "")
if(EXISTS "${KProperty_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    WORKING_DIRECTORY ${KProperty_SOURCE_DIR}
                    OUTPUT_VARIABLE KPROPERTY_GIT_SHA1_STRING)
    string(REGEX REPLACE "\n" "" KPROPERTY_GIT_SHA1_STRING "${KPROPERTY_GIT_SHA1_STRING}")
  endif()
endif()

if (BUILD_TESTING)
  add_definitions(-DBUILD_TESTING)
  include(CTest)
  add_subdirectory(autotests)
  #TODO add_subdirectory(tests)
else ()
  set(BUILD_COVERAGE OFF)
  simple_option(BUILD_COVERAGE "Build test coverage (disabled because BUILD_TESTING is OFF)" OFF)
endif ()

simple_option(BUILD_EXAMPLES "Build example applications" OFF)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_subdirectory(src)

simple_option(KPROPERTY_UNFINISHED
              "Include unfinished features (useful for testing but may confuse end-user)" OFF)

simple_option(BUILD_TESTING "Build tests" OFF) # override default from CTest.cmake

if (NOT WIN32)
  configure_file(${KProperty_SOURCE_DIR}/KProperty.pc.cmake ${KProperty_BINARY_DIR}/KProperty.pc @ONLY)
  install(FILES ${KProperty_BINARY_DIR}/KProperty.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
