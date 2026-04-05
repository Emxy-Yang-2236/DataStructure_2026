cmake_minimum_required(VERSION 3.28)
project(HashMap)

set(CMAKE_CXX_STANDARD 20)

add_executable(HashMap main.cpp
        simple_unordered_map.hpp
        simple_linked_hashmap.hpp
        dLinkList.hpp
        hybrid_hashmap.hpp
        unordered_map_with_iterator.hpp
        trial.cpp)
