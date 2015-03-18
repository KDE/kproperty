cmake_minimum_required(VERSION 3.0)
project(kproperty-tests)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

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
