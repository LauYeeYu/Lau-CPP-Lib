cmake_minimum_required(VERSION 3.16)
project(Lau-CPP-Lib)

set(CMAKE_CXX_STANDARD 17)

add_executable(test
        lau/algorithm.h
        lau/all.h
        lau/exception.h
        lau/file_data_structure.h
        lau/file_double_unrolled_linked_list.h
        lau/file_unrolled_linked_list.h
        lau/linked_hash_map.h
        lau/linked_hash_table.h
        lau/map.h
        lau/priority_queue.h
        lau/RB_tree.h
        lau/rehash_policy.cpp
        lau/rehash_policy.h
        lau/sparse_table.h
        lau/token_scanner.cpp
        lau/token_scanner.h
        lau/type_traits.h
        lau/utility.cpp
        lau/utility.h
        lau/vector.h
        test.cpp)
