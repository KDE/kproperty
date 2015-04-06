find_package(Qt5Widgets REQUIRED)
find_package(KF5WidgetsAddons)
find_package(KF5GuiAddons)

remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
)

set(kpropertyexample_SRCS main.cpp window.cpp)
add_executable(kpropertyexample ${kpropertyexample_SRCS})
target_link_libraries(kpropertyexample Qt5::Widgets KProperty)
target_include_directories(kpropertyexample PUBLIC ${CMAKE_BINARY_DIR}/src/kproperty)
