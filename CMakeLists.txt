cmake_minimum_required(VERSION 3.15)
project(websockets)

set(CMAKE_CXX_STANDARD 17)

# lind libpthread additionally
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(Boost 1.70 REQUIRED COMPONENTS system program_options)

add_executable(${CMAKE_PROJECT_NAME} main.cpp src/Websocket.cpp include/Websocket.h)
#add_executable(${CMAKE_PROJECT_NAME} some.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})

target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})