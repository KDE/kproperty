prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: KPropertyCore@PROJECT_STABLE_VERSION_MAJOR@
Description: A property editing framework (core library)
Version: @PROJECT_VERSION@
URL: https://community.kde.org/KProperty
Requires: Qt5Core
Libs: -L${libdir}
Cflags: -I${includedir}
