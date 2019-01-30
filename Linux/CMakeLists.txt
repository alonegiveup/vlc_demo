cmake_minimum_required (VERSION 2.8)

project (vlc_demo)

aux_source_directory(./src SRC_LIST)

# find required opencv
find_package(OpenCV REQUIRED)
# directory of opencv headers
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("/home/project/vlc_demo/include/")

add_executable(vlc_demo ${SRC_LIST})

include(CheckCXXCompilerFlag) 
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11) 
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X) 
if(COMPILER_SUPPORTS_CXX11) 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") 
elseif(COMPILER_SUPPORTS_CXX0X) 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x") 
else() 
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.") 
endif()

link_directories(${OpenCV_LIBRARY_DIRS})
link_directories("/home/project/vlc_demo/lib")

target_link_libraries(vlc_demo ${OpenCV_LIBS} libvlc.so.5.6.0 libvlccore.so.9.0.0)

