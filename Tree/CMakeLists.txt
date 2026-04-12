cmake_minimum_required(VERSION 3.28)
project(Tree)

set(CMAKE_CXX_STANDARD 20)

add_executable(Tree main.cpp
        BinarySearchTree.hpp
        Integer_Test.h
        AVLTree.h
        RedBlackTree.hpp)
