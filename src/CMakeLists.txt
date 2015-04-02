include(GenerateExportHeader)

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

include_directories(${KPROPERTY_INCLUDES})

########### next target ###############

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


add_library(kproperty SHARED ${kproperty_LIB_SRCS})
generate_export_header(kproperty)

target_link_libraries(kproperty KF5::WidgetsAddons KF5::GuiAddons)

#target_link_libraries(kproperty ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kowidgets)
#target_link_libraries(kproperty LINK_INTERFACE_LIBRARIES ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kowidgets)

set_target_properties(kproperty PROPERTIES VERSION 3.0 SOVERSION 3.0.0)
qt5_use_modules(kproperty Widgets)

install(TARGETS kproperty LIBRARY DESTINATION lib)

########### install files ###############

#kde4_install_icons(${DATA_INSTALL_DIR}/kproperty/icons)

if(TRUE) # TODO: install when we move to independent place
  install( FILES
    Factory.h
    Property.h
    Set.h
    Utils.h
    Factory.h
    EditorView.h
    ${PROJECT_BINARY_DIR}/src/kproperty_export.h
    DESTINATION include/KProperty COMPONENT Devel
  )

  install( FILES
    editors/utils.h
    editors/booledit.h
    editors/coloredit.h
    editors/combobox.h
    editors/cursoredit.h
    editors/dateedit.h
    editors/datetimeedit.h
    editors/dummywidget.h
    editors/fontedit.h
    editors/linestyleedit.h
    editors/pixmapedit.h
    editors/pointedit.h
    editors/rectedit.h
    editors/sizeedit.h
    editors/sizepolicyedit.h
    editors/spinbox.h
    editors/spinbox.h
    editors/stringedit.h
    editors/stringlistedit.h
    editors/symbolcombo.h
    editors/timeedit.h
    editors/urledit.h
  DESTINATION include/KProperty/editors COMPONENT Devel)
endif(TRUE)
