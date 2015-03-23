find_package(Qt5Widgets REQUIRED)
find_package(KF5WidgetsAddons)
find_package(KF5GuiAddons)

if(KPROPERTY_BUILD_TESTS)

    include_directories( ${CMAKE_SOURCE_DIR}/libs/koproperty ${KOPROPERTY_INCLUDES} )
    set(kpropertyexample_SRCS main.cpp window.cpp)
    add_executable(kpropertyexample ${kpropertyexample_SRCS})
    qt5_use_modules(kpropertyexample Widgets)
    target_link_libraries(kpropertyexample kproperty)

endif(KPROPERTY_BUILD_TESTS)
