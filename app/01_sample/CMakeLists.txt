cmake_minimum_required(VERSION 3.0)
project(demo)

# 设置编译类型
#-------------------------------------------------------------------------#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})
#-------------------------------------------------------------------------#

# 设置输出路径（需要放在 add_executable、add_library前面）
#-------------------------------------------------------------------------#
# # 设置可执行文件的输出路径
# SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#-------------------------------------------------------------------------#

# 设置语言
#-------------------------------------------------------------------------#
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
#-------------------------------------------------------------------------#

# 查找第三方库
#-------------------------------------------------------------------------#
#find OpenCV
# find_package(OpenCV 4.6.0 REQUIRED)
# if(NOT OpenCV_FOUND)
#    find_package(OpenCV 2.4.3 REQUIRED)
#    if(NOT OpenCV_FOUND)
#       message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
#    endif()
# endif()

# find fmt
find_package(fmt)
#-------------------------------------------------------------------------#

# 设置包含路径
#-------------------------------------------------------------------------#
include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include
# ${OpenCV_INCLUDE_DIR}
${fmt_INCLUDE_DIR}
)

# 生成库文件
#-------------------------------------------------------------------------#
# 设置DEBUG生成的库文件后缀
set(CMAKE_DEBUG_POSTFIX "_d")  
# 设置RELEASE生成的库文件后缀
set(CMAKE_RELEASE_POSTFIX "") 

# 设置库文件的输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# # 生成库文件
# add_library(${PROJECT_NAME} SHARED

# )

# 添加可执行文件
#-------------------------------------------------------------------------#
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
#-------------------------------------------------------------------------#