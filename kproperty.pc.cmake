prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KProperty
Description: A property editing framework with editor
Version: @KPROPERTY_VERSION@
URL: https://projects.kde.org/kproperty
Requires: QtCore QtGui KF5::WidgetsAddons KF5::GuiAddons
Libs: -L${libdir}
Cflags: -I${includedir}
