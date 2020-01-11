cmake_minimum_required(VERSION 3.15)
project(BookClient)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(StompBookClubClient src/main.cpp src/readFromKeyboardHandler.cpp src/readFromSocketHandler.cpp src/connectionHandler.cpp)
target_link_libraries(StompBookClubClient ${Boost_LIBRARIES})
target_link_libraries(StompBookClubClient Threads::Threads)
