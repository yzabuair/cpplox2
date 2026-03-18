cmake_minimum_required(VERSION 3.15)

project(
    cpplox2
    	LANGUAGES CXX
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libedit REQUIRED IMPORTED_TARGET libedit)

add_executable(
    cpplox2
        source/Chunk.h
        source/Common.h
        source/Compiler.cpp
        source/Compiler.h
        source/Debug.h
        source/Debug.cpp
        source/main.cpp 
        source/ParseError.h
        source/Scanner.cpp
        source/Scanner.h
        source/ScannerError.h
        source/Token.h
        source/TokenType.h
        source/Value.h
        source/Vm.cpp
        source/Vm.h
)

target_compile_features(
     cpplox2
         PUBLIC
         cxx_std_23
)

target_link_libraries(
    cpplox2
        PRIVATE
        PkgConfig::libedit
)
