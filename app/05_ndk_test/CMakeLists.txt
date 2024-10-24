cmake_minimum_required(VERSION 3.5)
project(demo)
# 设置编译类型
#-------------------------------------------------------------------------#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})
#-------------------------------------------------------------------------#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} main.cpp VideoTimingCalculator.cpp)

#链接到libandroid.so 与 liblog.so for logcat
target_link_libraries(${CMAKE_PROJECT_NAME} android log)