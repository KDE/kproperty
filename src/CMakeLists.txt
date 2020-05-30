# Options
# ...

set(kpropertycore_LIB_SRCS
   KProperty.cpp
   KPropertyComposedUrl.cpp
   KPropertyListData.cpp
   KPropertySet.cpp
   KPropertySetBuffer.cpp
   KPropertyFactory.cpp
   KPropertyCoreUtils.cpp
   kproperty_debug.cpp
)

set(kpropertycore_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ecm_create_qm_loader(kpropertycore_LIB_SRCS kpropertycore_qt)

add_library(KPropertyCore SHARED ${kpropertycore_LIB_SRCS})
set_coinstallable_lib_version(KPropertyCore)
target_link_libraries(KPropertyCore
    PUBLIC
        Qt5::Core
)
generate_export_header(KPropertyCore)

target_include_directories(KPropertyCore
    PUBLIC "$<BUILD_INTERFACE:${kpropertycore_INCLUDE_DIRS}>"
    INTERFACE "$<INSTALL_INTERFACE:${KPROPERTYCORE_INCLUDE_INSTALL_DIR}>"
)

if(KPROPERTY_WIDGETS)
    add_library(KPropertyUtilsPrivate STATIC
        KPropertyUtils_p.cpp
    )
    target_link_libraries(KPropertyUtilsPrivate
        PUBLIC
            Qt5::Widgets
    )

    add_subdirectory(editors)

    set(kpropertywidgets_LIB_SRCS
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
        editors/KPropertyRectFEditor.cpp
        editors/sizeedit.cpp
        editors/sizefedit.cpp
        editors/sizepolicyedit.cpp
        editors/spinbox.cpp
        editors/KPropertyComposedUrlEditor.cpp
        editors/KPropertyGenericSelectionEditor.cpp
        editors/KPropertyMultiLineStringEditor.cpp
        editors/KPropertyStringEditor.cpp
        editors/KPropertyUrlEditor.cpp
        editors/KPropertyUrlEditor_p.cpp
        editors/linestyleedit.cpp
        #  editors/stringlistedit.cpp
        #  editors/symbolcombo.cpp
        editors/timeedit.cpp

        kproperty_debug.cpp # needed here too
        KPropertyWidgetsFactory.cpp
        KPropertyWidgetsPluginManager.cpp
        KDefaultPropertyFactory.cpp
        KPropertyEditorItemEvent.cpp
        KPropertyEditorView.cpp
        KPropertyEditorDataModel_p.cpp
        KPropertyUtils.cpp
        KPropertyLineStyleSelector_p.cpp
        KPropertyLineStyleModel_p.cpp
        KPropertyLineStyleItemDelegate_p.cpp

        # non-source:
        Mainpage.dox
        Messages.sh
    )

     ecm_create_qm_loader(kpropertywidgets_LIB_SRCS kpropertywidgets_qt)

    set(kpropertywidgets_INCLUDE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}/editors
    )
    if(NOT KPROPERTY_KF)
        list(APPEND kpropertywidgets_LIB_SRCS
            editors/3rdparty/KColorCombo.cpp
            editors/3rdparty/KColorCollection.cpp
        )
        list(APPEND kpropertywidgets_INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/editors/3rdparty
        )
    endif()

    add_library(KPropertyWidgets SHARED ${kpropertywidgets_LIB_SRCS})
    set_coinstallable_lib_version(KPropertyWidgets)
    target_link_libraries(KPropertyWidgets
        PUBLIC
            KPropertyCore
            Qt5::Widgets
        PRIVATE
            KPropertyUtilsPrivate
    )
    if(KPROPERTY_KF)
        target_link_libraries(KPropertyUtilsPrivate
            PUBLIC
                KF5::ConfigGui #KConfigGroup
                KF5::GuiAddons #KColorCollection
                KF5::WidgetsAddons #KMessageBox
        )
    endif()
    generate_export_header(KPropertyWidgets)
    target_include_directories(KPropertyWidgets
        PUBLIC "$<BUILD_INTERFACE:${kpropertywidgets_INCLUDE_DIRS}>"
        INTERFACE "$<INSTALL_INTERFACE:${KPROPERTYWIDGETS_INCLUDE_INSTALL_DIR}>"
        PRIVATE editors
    )
endif()

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_CORE_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/${KPROPERTYCORE_BASE_NAME}")

# A place for KProperty plugins
set(KPROPERTY_PLUGIN_INSTALL_DIR ${PLUGIN_INSTALL_DIR}/${PROJECT_NAME_LOWER}${PROJECT_STABLE_VERSION_MAJOR})

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX KPROPERTYCORE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPropertyCoreConfigVersion.cmake"
)

configure_package_config_file(
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
    set(CMAKECONFIG_WIDGETS_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/${KPROPERTYWIDGETS_BASE_NAME}")

    ecm_setup_version(${PROJECT_VERSION}
        VARIABLE_PREFIX KPROPERTYWIDGETS
        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h"
        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPropertyWidgetsConfigVersion.cmake"
    )
    configure_package_config_file(
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

if(KPROPERTY_GENERATE_PRI)
    ecm_generate_pri_file(
        BASE_NAME ${KPROPERTYCORE_BASE_NAME}
        LIB_NAME ${KPROPERTYCORE_BASE_NAME}
        DEPS "core"
        FILENAME_VAR PRI_FILENAME
        INCLUDE_INSTALL_DIR ${KPROPERTYCORE_INCLUDE_INSTALL_DIR}
    )
    if(KPROPERTY_WIDGETS)
        ecm_generate_pri_file(
            BASE_NAME ${KPROPERTYWIDGETS_BASE_NAME}
            LIB_NAME ${KPROPERTYWIDGETS_BASE_NAME}
            DEPS "widgets KPropertyCore"
            FILENAME_VAR PRI_FILENAME
            INCLUDE_INSTALL_DIR ${KPROPERTYWIDGETS_INCLUDE_INSTALL_DIR}
        )
    endif()
endif()

#ecm_install_icons(${DATA_INSTALL_DIR}/kproperty/icons)

ecm_generate_headers(kpropertycore_FORWARDING_HEADERS
    REQUIRED_HEADERS kpropertycore_HEADERS
    ORIGINAL CAMELCASE
    HEADER_NAMES
        KProperty
        KPropertyComposedUrl
        KPropertyListData
        KPropertySet
        KPropertySetBuffer
        KPropertyFactory
)

list(APPEND kpropertycore_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/kproperty_version.h)

install(
    FILES
        ${kpropertycore_HEADERS}
        ${kpropertycore_FORWARDING_HEADERS}
        ${PROJECT_BINARY_DIR}/src/kpropertycore_export.h
        ${PROJECT_BINARY_DIR}/src/config-kproperty.h
    DESTINATION ${KPROPERTYCORE_INCLUDE_INSTALL_DIR}
    COMPONENT Devel
)

if(KPROPERTY_WIDGETS)
    add_subdirectory(pics)

    ecm_generate_headers(kpropertywidgets_FORWARDING_HEADERS
        REQUIRED_HEADERS kpropertywidgets_HEADERS
        ORIGINAL CAMELCASE
        HEADER_NAMES
            KPropertyWidgetsFactory
            KPropertyWidgetsPluginManager
            KPropertyUtils
            KPropertyEditorItemEvent
            KPropertyEditorView
    )

    install(
        FILES
            ${kpropertywidgets_HEADERS}
            ${kpropertywidgets_FORWARDING_HEADERS}
            ${kproperty_editors_HEADERS}
            ${kproperty_editors_FORWARDING_HEADERS}
            ${PROJECT_BINARY_DIR}/src/kpropertywidgets_export.h
        DESTINATION ${KPROPERTYWIDGETS_INCLUDE_INSTALL_DIR}
        COMPONENT Devel
    )
endif()

if(BUILD_QCH)
    ecm_add_qch(
        KPropertyCore_QCH
        NAME KPropertyCore
        BASE_NAME ${KPROPERTYCORE_BASE_NAME}
        VERSION ${PROJECT_VERSION}
        ORG_DOMAIN org.kde.${KPROPERTYCORE_BASE_NAME}
        SOURCES
            Mainpage.dox
            ${kpropertycore_HEADERS}
        LINK_QCHS
            Qt5Core_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
            ${kpropertycore_INCLUDE_DIRS}
        BLANK_MACROS
            KPROPERTYCORE_EXPORT
            KPROPERTYCORE_DEPRECATED
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
    set(kpropertycore_qch_targets KPropertyCore_QCH)
endif()

ecm_install_qch_export(
    TARGETS ${kpropertycore_qch_targets}
    FILE KPropertyCoreQCHTargets.cmake
    DESTINATION "${CMAKECONFIG_CORE_INSTALL_DIR}"
    COMPONENT Devel
)

if(KPROPERTY_WIDGETS)
    if(BUILD_QCH)
        if(KPROPERTY_KF)
            set(_KF5WidgetsAddons_QCH KF5WidgetsAddons_QCH)
        endif()
        ecm_add_qch(
            KPropertyWidgets_QCH
            NAME KPropertyWidgets
            BASE_NAME ${KPROPERTYWIDGETS_BASE_NAME}
            VERSION ${PROJECT_VERSION}
            ORG_DOMAIN org.kde.${KPROPERTYWIDGETS_BASE_NAME}
            SOURCES
                ${kpropertywidgets_HEADERS}
                ${kproperty_editors_HEADERS}
            LINK_QCHS
                Qt5Core_QCH
                Qt5Gui_QCH
                Qt5Widgets_QCH
                KPropertyCore_QCH
                ${_KF5WidgetsAddons_QCH}
            INCLUDE_DIRS
                ${CMAKE_CURRENT_BINARY_DIR}
                ${kpropertywidgets_INCLUDE_DIRS}
            BLANK_MACROS
                KPROPERTYWIDGETS_EXPORT
                KPROPERTYWIDGETS_DEPRECATED
            TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
            QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
            COMPONENT Devel
        )
        set(kpropertywidgets_qch_targets KPropertyWidgets_QCH)
    endif()

    ecm_install_qch_export(
        TARGETS ${kpropertywidgets_qch_targets}
        FILE KPropertyWidgetsQCHTargets.cmake
        DESTINATION "${CMAKECONFIG_WIDGETS_INSTALL_DIR}"
        COMPONENT Devel
    )
endif()

enable_testing()

configure_file(config-kproperty.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kproperty.h)
