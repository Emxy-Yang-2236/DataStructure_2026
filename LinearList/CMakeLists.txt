# cmake_minimum_required(VERSION <specify CMake version here>)
project(LinearList)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LinearList
    dLinkList.hpp
    main.cpp
    seqList.hpp
    sLinkList.hpp)
