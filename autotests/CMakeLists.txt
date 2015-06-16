remove_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_SIGNALS_SLOTS_KEYWORDS
    -DQT_NO_CAST_FROM_ASCII
    -DQT_USE_QSTRINGBUILDER
)

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

add_subdirectory(headers)
