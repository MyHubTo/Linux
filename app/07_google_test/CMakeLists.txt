cmake_minimum_required(VERSION 3.10)
project(DEMO
    VERSION 1.0.0
    LANGUAGES CXX C)

include(GNUInstallDirs)

string(TOLOWER ${PROJECT_NAME} LIBRARY_NAME)

option(DEMO_TEST "Generate the test target." ON)
option(DEMO_FMT "Using fmt library." ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
if (DEMO_TEST)
  enable_testing()
  add_subdirectory(test)
  message("test enable ...")
endif ()
add_subdirectory(libs/googletest)
