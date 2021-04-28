cmake_minimum_required(VERSION 3.15)
project(sfq_parser)

set(CMAKE_CXX_STANDARD 11)

add_executable(sfq_parser
        main.cpp
        circuit.cpp circuit.h
        node.cpp node.h
        circuit_Quine_McCluskey.cpp
        circuit_sim.cpp
        )