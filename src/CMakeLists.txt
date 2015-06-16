# Options
# simple_option(KPROPERTY_...... "....." OFF)

configure_file(config-kproperty.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kproperty.h)

add_subdirectory(editors)

set(libkpropertyeditors_SRCS
  editors/utils.cpp
  editors/booledit.cpp
  editors/coloredit.cpp
  editors/combobox.cpp
  editors/cursoredit.cpp
  editors/dateedit.cpp
  editors/datetimeedit.cpp
#  editors/dummywidget.cpp
  editors/fontedit.cpp
  editors/pixmapedit.cpp
  editors/pointedit.cpp
  editors/pointfedit.cpp
  editors/rectedit.cpp
  editors/sizeedit.cpp
  editors/sizefedit.cpp
  editors/sizepolicyedit.cpp
  editors/spinbox.cpp
  editors/KPropertyStringEditor.cpp
  editors/linestyleedit.cpp
#  editors/stringlistedit.cpp
#  editors/symbolcombo.cpp
  editors/timeedit.cpp
#TODO  editors/urledit.cpp
)

set(kproperty_LIB_SRCS
   kproperty_debug.cpp
   KProperty.cpp
   KPropertySet.cpp
   KPropertyFactory.cpp
   KDefaultPropertyFactory.cpp
   KPropertyEditorView.cpp
   KPropertyEditorDataModel.cpp
   KPropertyUtils.cpp
   KPropertyUnit_p.cpp
   KPropertyLineStyleSelector_p.cpp
   KPropertyLineStyleModel_p.cpp
   KPropertyLineStyleItemDelegate_p.cpp
   ${libkpropertyeditors_SRCS}
)

add_library(KProperty SHARED ${kproperty_LIB_SRCS})

generate_export_header(KProperty)

set(kproperty_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/kproperty
    ${CMAKE_CURRENT_BINARY_DIR}/editors
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/editors
)

target_include_directories(KProperty
    PUBLIC "$<BUILD_INTERFACE:${kproperty_INCLUDE_DIRS}>"
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KProperty>"
    PRIVATE editors
)

target_link_libraries(KProperty
    KF5::WidgetsAddons
    KF5::GuiAddons
    #TODO KF5::KIOWidgets
)

set_target_properties(KProperty
    PROPERTIES VERSION 1.0
               SOVERSION 1.0.0
               EXPORT_NAME KProperty
)

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KProperty")

# A place for KProperty plugins
set(KPROPERTY_PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_DIR}/kproperty)

ecm_setup_version(${KPROPERTY_VERSION}
    VARIABLE_PREFIX KPROPERTY
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPropertyConfigVersion.cmake"
)

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPropertyConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPropertyConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/KPropertyConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/KPropertyConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(TARGETS KProperty
        EXPORT KPropertyTargets
        ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(EXPORT KPropertyTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KPropertyTargets.cmake)

ecm_generate_pri_file(
    BASE_NAME KProperty
    LIB_NAME KProperty
    DEPS "widgets"
    FILENAME_VAR PRI_FILENAME
    INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/KProperty
)

install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

#ecm_install_icons(${DATA_INSTALL_DIR}/kproperty/icons)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT Devel)

ecm_generate_headers(kproperty_FORWARDING_HEADERS
    REQUIRED_HEADERS kproperty_HEADERS
    ORIGINAL CAMELCASE
    PREFIX kproperty
    HEADER_NAMES
        KPropertyFactory
        KProperty
        KPropertySet
        KPropertyUtils
        KPropertyFactory
        KPropertyEditorView
        KPropertyEditorDataModel
)

install(
    FILES
        ${kproperty_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty/kproperty
    COMPONENT Devel
)

install(
    FILES
        ${kproperty_FORWARDING_HEADERS}
        ${PROJECT_BINARY_DIR}/src/kproperty_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty
    COMPONENT Devel
)
