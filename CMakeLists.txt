cmake_minimum_required(VERSION 3.15)
project(huffman C)

set(CMAKE_C_STANDARD 99)

add_executable(huffman main.c huffman.h dec.c enc.c)