cmake_minimum_required(VERSION 3.15)

project(
    cpplox2
    	LANGUAGES CXX
)

add_executable(
    cpplox2
        source/chunk.h
        source/common.h
        source/debug.h
        source/debug.cpp
        source/main.cpp
        source/value.h
        source/vm.cpp
        source/vm.h
)

target_compile_features(
     cpplox2
         PUBLIC
         cxx_std_23
)
