find_package(Qt5Widgets REQUIRED)
find_package(KF5WidgetsAddons)
find_package(KF5GuiAddons)

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
#  editors/spinbox.cpp
  editors/stringedit.cpp
  editors/linestyleedit.cpp
#  editors/stringlistedit.cpp
#  editors/symbolcombo.cpp
  editors/timeedit.cpp
#  editors/urledit.cpp
)

set(kproperty_LIB_SRCS
   Property.cpp
   Set.cpp
   Factory.cpp
   DefaultFactory.cpp
   EditorView.cpp
   EditorDataModel.cpp
   Utils.cpp
   KoLineStyleSelector.cpp
   KoUnit.cpp
   KoLineStyleModel.cpp
   KoLineStyleItemDelegate.cpp
   ${libkpropertyeditors_SRCS}
)

add_library(KProperty SHARED ${kproperty_LIB_SRCS})

generate_export_header(KProperty)

target_include_directories(KProperty INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KProperty>")

target_link_libraries(KProperty KF5::WidgetsAddons KF5::GuiAddons)

#target_link_libraries(kproperty ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kowidgets)
#target_link_libraries(kproperty LINK_INTERFACE_LIBRARIES ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kowidgets)

set_target_properties(KProperty
    PROPERTIES VERSION 1.0
               SOVERSION 1.0.0
               EXPORT_NAME KProperty
)

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KProperty")

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
        ${INSTALL_TARGETS_DEFAULT_ARGS})

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

#kde4_install_icons(${DATA_INSTALL_DIR}/kproperty/icons)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT Devel)

ecm_generate_headers(kproperty_FORWARDING_HEADERS
    ORIGINAL CAMELCASE
    PREFIX kproperty
    HEADER_NAMES
        Factory
        Property
        Set
        Utils
        Factory
        EditorView
    REQUIRED_HEADERS kproperty_HEADERS
)

install(
    FILES
        ${kproperty_HEADERS}
        ${PROJECT_BINARY_DIR}/src/kproperty_export.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty/kproperty
    COMPONENT Devel
)

install(
    FILES
        ${kproperty_FORWARDING_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KProperty/KProperty
    COMPONENT Devel
)

add_subdirectory(editors)
