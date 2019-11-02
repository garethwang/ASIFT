cmake_minimum_required(VERSION 2.8) 
 
project(Demo_ASIFT) 

if (CMAKE_COMPILER_IS_GNUCXX)

	set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -O3")

endif (CMAKE_COMPILER_IS_GNUCXX)

add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(SRC_DIR ${CMAKE_SOURCE_DIR}/libAsift)

file(GLOB_RECURSE HEADER_FILES ${SRC_DIR}/*.h ${SRC_DIR}/*.hpp)

file(GLOB_RECURSE SOURCE_FILES ${SRC_DIR}/*.cpp)

include_directories(${SRC_DIR})

add_executable(demo_asift demo_asift.cpp ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(demo_asift ${OpenCV_LIBS})

