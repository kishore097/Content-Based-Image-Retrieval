cmake_minimum_required(VERSION 2.8)
project(CV_PROJECT_2)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(CV_PROJECT_2 imgDisp.cpp )


# target_link_libraries(CV_PROJECT_2 )
target_link_libraries(CV_PROJECT_2 ${OpenCV_LIBS})