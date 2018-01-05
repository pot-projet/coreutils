cmake_minimum_required(VERSION 3.8)
project(coreutils-cat)

set(SOURCE_FILES main.cpp ../include/coreutils/param.hpp)
add_executable(cat ${SOURCE_FILES})
