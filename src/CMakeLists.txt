# Options
# simple_option(KPROPERTY_...... "....." OFF)
configure_file(config-kproperty.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kproperty.h)

set(kpropertycore_LIB_SRCS
   KProperty.cpp
   KPropertySet.cpp
   KPropertyUnit_p.cpp
   KPropertyFactory.cpp
   kproperty_debug.cpp
)

if(KPROPERTY_WIDGETS)
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

    set(kpropertywidgets_LIB_SRCS
    ${libkpropertyeditors_SRCS}
    kpropertywidgets_debug.cpp
    KPropertyWidgetsFactory.cpp
    KDefaultPropertyFactory.cpp
    KPropertyEditorView.cpp
    KPropertyEditorDataModel.cpp
    KPropertyUtils.cpp
    KPropertyLineStyleSelector_p.cpp
    KPropertyLineStyleModel_p.cpp
    KPropertyLineStyleItemDelegate_p.cpp
    )
endif()

set(kproperty_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}/kproperty
    ${CMAKE_CURRENT_BINARY_DIR}/editors
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/editors
)

ecm_create_qm_loader(kpropertycore_LIB_SRCS kpropertycore_qt)

add_library(KPropertyCore SHARED ${kpropertycore_LIB_SRCS})
target_link_libraries(KPropertyCore
    Qt5::Core
)
generate_export_header(KPropertyCore)

target_include_directories(KPropertyCore
    PUBLIC "$<BUILD_INTERFACE:${kproperty_INCLUDE_DIRS}>"
    INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KPropertyCore>"
    PRIVATE editors
)

set_target_properties(KPropertyCore
    PROPERTIES VERSION 1.0
               SOVERSION 1.0.0
               EXPORT_NAME KPropertyCore
)

if(KPROPERTY_WIDGETS)
    add_library(KPropertyWidgets SHARED ${kpropertywidgets_LIB_SRCS})
    target_link_libraries(KPropertyWidgets
	KPropertyCore
        KF5::WidgetsAddons
        KF5::GuiAddons
    )
    generate_export_header(KPropertyWidgets)
    target_include_directories(KPropertyWidgets
	PUBLIC "$<BUILD_INTERFACE:${kproperty_INCLUDE_DIRS}>"
	INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KPropertyWidgets>"
	PRIVATE editors
    )

    set_target_properties(KPropertyWidgets
	PROPERTIES VERSION 1.0
		  SOVERSION 1.0.0
		  EXPORT_NAME KPropertyWidgets
    )
endif()

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_CORE_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KPropertyCore")

# A place for KProperty plugins
set(KPROPERTY_PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_DIR}/kproperty)

ecm_setup_version(${KPROPERTY_VERSION}
    VARIABLE_PREFIX KPROPERTY
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPropertyCoreConfigVersion.cmake"
)

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPropertyCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPropertyCoreConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_CORE_INSTALL_DIR}"
)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/KPropertyCoreConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/KPropertyCoreConfigVersion.cmake"
            DESTINATION "${CMAKECONFIG_CORE_INSTALL_DIR}"
            COMPONENT Devel)

install(TARGETS KPropertyCore
            EXPORT KPropertyCoreTargets
            ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
            
install(EXPORT KPropertyCoreTargets
	DESTINATION "${CMAKECONFIG_CORE_INSTALL_DIR}"
	FILE KPropertyCoreTargets.cmake)
            
if(KPROPERTY_WIDGETS)
  set(CMAKECONFIG_WIDGETS_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KPropertyWidgets")

    ecm_setup_version(${KPROPERTY_VERSION}
	VARIABLE_PREFIX KPROPERTY
	VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
	PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPropertyWidgetsConfigVersion.cmake"
    )
    ecm_configure_package_config_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/KPropertyWidgetsConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/KPropertyWidgetsConfig.cmake"
	INSTALL_DESTINATION "${CMAKECONFIG_WIDGETS_INSTALL_DIR}"
    )
    
    install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/KPropertyWidgetsConfig.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/KPropertyWidgetsConfigVersion.cmake"
	DESTINATION "${CMAKECONFIG_WIDGETS_INSTALL_DIR}"
	COMPONENT Devel)

    install(TARGETS KPropertyWidgets
            EXPORT KPropertyWidgetsTargets
            ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
            
    install(EXPORT KPropertyWidgetsTargets
	DESTINATION "${CMAKECONFIG_WIDGETS_INSTALL_DIR}"
	FILE KPropertyWidgetsTargets.cmake)

endif()
            
install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

ecm_generate_pri_file(
    BASE_NAME KPropertyCore
    LIB_NAME KPropertyCore
    DEPS "core"
    FILENAME_VAR PRI_FILENAME
    INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/KPropertyCore
)
    
if(KPROPERTY_WIDGETS)   
    ecm_generate_pri_file(
	BASE_NAME KPropertyWidgets
	LIB_NAME KPropertyWidgets
	DEPS ""
	FILENAME_VAR PRI_FILENAME
	INCLUDE_INSTALL_DIR ${INCLUDE_INSTALL_DIR}/KPropertyWidgets
    )
endif()

#ecm_install_icons(${DATA_INSTALL_DIR}/kproperty/icons)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
        DESTINATION "${INCLUDE_INSTALL_DIR}"
        COMPONENT Devel)

ecm_generate_headers(kpropertycore_FORWARDING_HEADERS
    REQUIRED_HEADERS kpropertycore_HEADERS
    ORIGINAL CAMELCASE
    HEADER_NAMES
        KProperty
        KPropertySet
        KPropertyFactory
)

install(
    FILES
        ${kpropertycore_HEADERS}
    DESTINATION ${INCLUDE_INSTALL_DIR}/KPropertyCore
    COMPONENT Devel
)

install(
    FILES
        ${kpropertycore_FORWARDING_HEADERS}
        ${PROJECT_BINARY_DIR}/src/kpropertycore_export.h
        ${PROJECT_BINARY_DIR}/src/config-kproperty.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/KPropertyCore
    COMPONENT Devel
)

if(KPROPERTY_WIDGETS)  
    ecm_generate_headers(kpropertywidgets_FORWARDING_HEADERS
	REQUIRED_HEADERS kpropertywidgets_HEADERS
	ORIGINAL CAMELCASE
	HEADER_NAMES
	    KPropertyWidgetsFactory
	    KPropertyUtils
	    KPropertyEditorView
	    KPropertyEditorDataModel
    )

    install(
	FILES
	    ${kpropertywidgets_HEADERS}
	DESTINATION ${INCLUDE_INSTALL_DIR}/KPropertyWidgets
	COMPONENT Devel
    )

    install(
	FILES
	    ${kpropertywidgets_FORWARDING_HEADERS}
	    ${PROJECT_BINARY_DIR}/src/kpropertywidgets_export.h
	    ${PROJECT_BINARY_DIR}/src/config-kproperty.h
	DESTINATION ${INCLUDE_INSTALL_DIR}/KPropertyWidgets
	COMPONENT Devel
    )
endif()

enable_testing()
