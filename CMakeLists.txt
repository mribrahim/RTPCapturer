cmake_minimum_required(VERSION 2.8)
project (RTPCapturer)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# The following folder will be included
#include_directories("${PROJECT_SOURCE_DIR}")
include_directories(C:/Users/IbrahimD/Desktop/RTPCapturer-master/WpdPack/Include)

#link_directories(${CMAKE_SOURCE_DIR}//WpdPack/Lib)
link_directories(C:/Users/IbrahimD/Desktop/RTPCapturer-master/WpdPack/Lib)


file(GLOB SOURCES *.cpp)
#set(SOURCES "Tutorial.cpp")


add_executable(RTPCapturer ${SOURCES} )

TARGET_LINK_LIBRARIES(RTPCapturer wpcap)