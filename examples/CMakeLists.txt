remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
)

set(kpropertyexample_SRCS main.cpp window.cpp)
add_executable(kpropertyexample ${kpropertyexample_SRCS})
target_link_libraries(kpropertyexample
    KPropertyWidgets
)
if(KPROPERTY_KF)
    target_link_libraries(kpropertyexample
        KF5::ConfigGui #KConfigGroup
    )
endif()
