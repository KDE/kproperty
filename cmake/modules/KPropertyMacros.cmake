# Additional CMake macros
#
# Copyright (C) 2015 Jarosław Staniek <staniek@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FeatureSummary)
include(GetGitRevisionDescription)

# Adds a feature info using add_feature_info() with _NAME and _DESCRIPTION.
# If _NAME is equal to _DEFAULT, shows this fact.
macro(add_simple_feature_info _NAME _DESCRIPTION _DEFAULT)
  if("${_DEFAULT}" STREQUAL "${${_NAME}}")
    set(_STATUS " (default value)")
  else()
    set(_STATUS "")
  endif()
  add_feature_info(${_NAME} ${_NAME} ${_DESCRIPTION}${_STATUS})
endmacro()

# Adds a simple option using option() with _NAME and _DESCRIPTION and a feature
# info for it using add_simple_feature_info(). If _NAME is equal to _DEFAULT, shows this fact.
macro(simple_option _NAME _DESCRIPTION _DEFAULT)
  option(${_NAME} ${_DESCRIPTION} ${_DEFAULT})
  add_simple_feature_info(${_NAME} ${_DESCRIPTION} ${_DEFAULT})
endmacro()

# Fetches git revision and branch from the source dir of the current build if possible.
# Sets ${PROJECT_NAME_UPPER}_GIT_SHA1_STRING and ${PROJECT_NAME_UPPER}_GIT_BRANCH_STRING variables.
macro(get_git_revision_and_branch)
  get_git_head_revision(GIT_REFSPEC _GIT_SHA1)
  get_git_branch(_GIT_BRANCH)
  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
  if(_GIT_SHA1 AND _GIT_BRANCH)
    string(SUBSTRING ${_GIT_SHA1} 0 7 _GIT_SHA1)
    set(${PROJECT_NAME_UPPER}_GIT_SHA1_STRING ${_GIT_SHA1})
    set(${PROJECT_NAME_UPPER}_GIT_BRANCH_STRING ${_GIT_BRANCH})
  else()
    set(${PROJECT_NAME_UPPER}_GIT_SHA1_STRING "")
    set(${PROJECT_NAME_UPPER}_GIT_BRANCH_STRING "")
  endif()
endmacro()

# Adds BUILD_TESTING option to enable all kinds of tests. If enabled, build in autotests/
# and tests/ subdirectory is enabled. IF optional argument ARG1 is ON, building tests will
# be ON by default. Otherwise building tests will be OFF. ARG1 is OFF by default.
# If tests are OFF, BUILD_COVERAGE is set to OFF.
# If tests are on BUILD_TESTING macro is defined.
macro(add_tests)
  if (NOT "${ARG1}" STREQUAL "ON")
    set(_SET OFF)
  endif()
  simple_option(BUILD_TESTING "Build tests" ${_SET}) # override default from CTest.cmake
  if(BUILD_TESTING)
    add_definitions(-DBUILD_TESTING)
    include(CTest)
    if (EXISTS ${CMAKE_SOURCE_DIR}/autotests)
        add_subdirectory(autotests)
    endif()
    if (EXISTS ${CMAKE_SOURCE_DIR}/tests)
        add_subdirectory(tests)
    endif()
  else()
    set(BUILD_COVERAGE OFF)
    simple_option(BUILD_COVERAGE "Build test coverage (disabled because BUILD_TESTING is OFF)" OFF)
  endif()
endmacro()

# Adds BUILD_EXAMPLES option to enable examples. If enabled, build in examples/ subdirectory
# is enabled. If optional argument ARG1 is ON, building examples will be ON by default.
# Otherwise building examples will be OFF. ARG1 is OFF by default.
macro(add_examples)
  set(_SET ${ARGV0})
  if (NOT "${_SET}" STREQUAL ON)
    set(_SET OFF)
  endif()
  simple_option(BUILD_EXAMPLES "Build example applications" ${_SET})
  if (BUILD_EXAMPLES AND EXISTS ${CMAKE_SOURCE_DIR}/examples)
    add_subdirectory(examples)
  endif()
endmacro()

# Adds ${PROJECT_NAME_UPPER}_UNFINISHED option. If it is ON, unfinished features
# (useful for testing but may confuse end-user) are compiled-in.
# This option is OFF by default.
macro(add_unfinished_features_option)
  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
  simple_option(${PROJECT_NAME_UPPER}_UNFINISHED
                "Include unfinished features (useful for testing but may confuse end-user)" OFF)
endmacro()

# Adds commands that generate ${PROJECT_NAME}.pc file out of ${PROJECT_NAME}.pc.cmake file
# and install the .pc file to ${LIB_INSTALL_DIR}/pkgconfig. These commands are not executed for WIN32.
# ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.pc.cmake should exist.
macro(add_pc_file)
  if (NOT WIN32)
    configure_file(${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.pc.cmake ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
  endif()
endmacro()
