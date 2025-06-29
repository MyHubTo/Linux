cmake_minimum_required(VERSION 3.10)
project(demo
    VERSION 1.0.0
    LANGUAGES CXX C)

include(GNUInstallDirs)
include(CMakeDependentOption)

set(CMAKE_BUILD_TYPE "Debug")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror=return-type -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror=return-type -O3 -march=native")

# Check C++17 or C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX17)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
   add_definitions(-DCOMPILEDWITHC17)
   message(STATUS "Using flag -std=c++17.")
elseif(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

find_package(fmt CONFIG REQUIRED)
if(NOT fmt_FOUND)
   message("fmt not found\n")
elseif(fmt_FOUND)
   message("fmt found successful\n")
   include_directories(
    ${fmt_INCLUDE_DIR}
   )
endif()
find_package(Poco REQUIRED COMPONENTS Foundation Net Util)
add_executable(sender sender.cpp)
target_link_libraries(sender PUBLIC fmt::fmt ${Poco_LIBRARIES})

add_executable(receiver receiver.cpp)
target_link_libraries(receiver PUBLIC fmt::fmt ${Poco_LIBRARIES})
message("POCO_LIBRARIES: ${Poco_LIBRARIES}")


