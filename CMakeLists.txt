cmake_minimum_required(VERSION 3.15)
project(mapping)

set(CMAKE_CXX_STANDARD 11)

add_executable(mapping
        main.cpp
        circuit.cpp circuit.h
        node.cpp node.h
        circuit_Quine_McCluskey.cpp
        circuit_sim.cpp
        circuit_replace.cpp
        circuit_util.cpp
        )