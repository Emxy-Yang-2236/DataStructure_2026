cmake_minimum_required(VERSION 3.28)
project(Stack)

set(CMAKE_CXX_STANDARD 20)

add_executable(Stack main.cpp
        src/BaseClass.hpp
        src/SeqStack.hpp
        main.cpp
        src/LinkStack.hpp
        UtilizationOfStack/QuickSort.cpp
        UtilizationOfStack/InfixToPrefix.cpp)
