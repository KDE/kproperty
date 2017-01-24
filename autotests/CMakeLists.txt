remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
    -DQT_USE_QSTRINGBUILDER
)

include(ECMAddTests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(FILES_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(
    -DFILES_OUTPUT_DIR=\"${FILES_OUTPUT_DIR}\"
)

include_directories(
    ${CMAKE_BINARY_DIR}/src/kproperty
    ${CMAKE_BINARY_DIR}/src/editors/kproperty
)

include(ECMAddTests)

find_package(Qt5Test REQUIRED)

ecm_add_tests(
    KPropertyTest.cpp
    KPropertySetTest.cpp
    LINK_LIBRARIES
        Qt5::Test
        KPropertyWidgets
)

if(NOT WIN32) #TODO enable for Windows when headers_test.sh is ported e.g. to python
    add_subdirectory(headers)
endif()
