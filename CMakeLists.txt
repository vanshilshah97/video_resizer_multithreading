cmake_minimum_required(VERSION 2.8)
project( takehome )

set (CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} include)
add_executable(takehome main.cpp src/VideoEditor.cpp)
target_link_libraries( takehome ${OpenCV_LIBS} )
target_link_libraries( takehome Threads::Threads)