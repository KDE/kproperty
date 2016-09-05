prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KPropertyWidgets@PROJECT_STABLE_VERSION_MAJOR@
Description: A property editing framework with editor (Qt Widgets library)
Version: @PROJECT_VERSION@
URL: https://community.kde.org/KProperty
Requires: KPropertyCore Qt5Core Qt5Gui KF5WidgetsAddons KF5GuiAddons
Libs: -L${libdir}
Cflags: -I${includedir}
