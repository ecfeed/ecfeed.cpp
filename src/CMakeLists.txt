cmake_minimum_required(VERSION 3.5)
project(ecfeed)

SET_TARGET_PROPERTIES(ecfeed PROPERTIES LINKER_LANGUAGE C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(ecfeed ecfeed.hpp)