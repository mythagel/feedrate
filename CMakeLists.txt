CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(feedrate)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

ADD_DEFINITIONS(-Wno-multichar)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++14")

ADD_EXECUTABLE(simplex simplex.cpp)
ADD_LIBRARY(feedrate STATIC feedrate.cpp utils.cpp)

ADD_EXECUTABLE(test_feedrate main.cpp)
TARGET_LINK_LIBRARIES(test_feedrate feedrate)
