cmake_minimum_required(VERSION 3.16)
project(Lau-CPP-Lib)

set(CMAKE_CXX_STANDARD 17)

add_executable(test
        lau/all.h
        lau/file_double_unrolled_linked_list.h
        lau/file_unrolled_linked_list.h
        lau/token_scanner.h
        lau/token_scanner.cpp
        lau/utility.h
        lau/utility.cpp
        test.cpp lau/file_data_structure.h)
