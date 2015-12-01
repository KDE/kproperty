prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KPropertyCore
Description: A property editing framework (core library)
Version: @KPROPERTY_VERSION@
URL: https://community.kde.org/KProperty
Requires: Qt5Core
Libs: -L${libdir}
Cflags: -I${includedir}
