prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KProperty
Description: A property editing framework with editor
Version: @KPROPERTY_VERSION@
URL: https://community.kde.org/KProperty
Requires: Qt5Core Qt5Gui KF5WidgetsAddons KF5GuiAddons
Libs: -L${libdir}
Cflags: -I${includedir}
