cmake_minimum_required(VERSION 3.16)
project(Lau-CPP-Lib)

set(CMAKE_CXX_STANDARD 17)

add_executable(test
        test.cpp
        lau/token_scanner.h
        lau/token_scanner.cpp
        lau/utility.h
        lau/utility.cpp
        lau/all.h)
