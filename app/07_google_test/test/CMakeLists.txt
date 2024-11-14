set(BINARY ${CMAKE_PROJECT_NAME}_test)
message("${CMAKE_PROJECT_NAME}")

list(APPEND TEST_SOURCES
    test_main.cpp
)

find_package(fmt)
include_directories(
    ${fmt_INCLUDE_DIR}
)

add_executable(${BINARY} ${TEST_SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC ${LIBRARY_NAME} gtest fmt::fmt)