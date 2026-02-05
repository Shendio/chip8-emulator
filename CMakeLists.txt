cmake_minimum_required(VERSION 3.28)

project(chip8-emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)

add_executable(chip8-emulator src/main.cpp)
target_compile_features(chip8-emulator PRIVATE cxx_std_23)

set_target_properties(chip8-emulator PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
