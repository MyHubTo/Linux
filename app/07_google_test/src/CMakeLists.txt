cmake_minimum_required(VERSION 3.14)
## Library sources
macro( append_glob list glob )
    file(GLOB files ${glob})
    set(${list} "${${list}};${files}")
endmacro()

## Header only includes / core headers
set( INCDIR "${PROJECT_SOURCE_DIR}/include" )
list(APPEND LIB_INC_DIR  "${PROJECT_SOURCE_DIR}/include;${CMAKE_CURRENT_BINARY_DIR}/include" )

append_glob(HEADERS ${INCDIR}/*.h*)
append_glob(HEADERS ${INCDIR}/log/*.h*)

### Store list of source files
append_glob(SOURCES *.cpp)
append_glob(SOURCES log/*.cpp)

list(APPEND HEADERS
    ${INCDIR}/video/video.h
)

list(APPEND SOURCES
    video/video.cpp
)

if(ANDROID)
    # Fix issue with thread local storage on android.
    add_definitions(-fno-data-sections)
    list(APPEND LINK_LIBS android log)
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

if(DEMO_FMT)
    message("using fmt ...\n")
    list(APPEND LINK_LIBS fmt::fmt)
endif()

include_directories(${LIB_INC_DIR} ${USER_INC})

add_library(${LIBRARY_NAME} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${LIBRARY_NAME} PUBLIC ${LINK_LIBS})

add_executable(${PROJECT_NAME}_main ${SOURCES} ${HEADERS} main.cpp)
target_link_libraries(${PROJECT_NAME}_main PUBLIC ${LINK_LIBS})

## set_property示例
set_property(TARGET ${PROJECT_NAME}_main
    PROPERTY
    POSITION_INDEPENDENT_CODE ON
)

## set_target_properties
set_target_properties (${LIBRARY_NAME}
    PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)