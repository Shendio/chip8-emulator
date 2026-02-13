cmake_minimum_required(VERSION 3.28)

project(chip8-emulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

find_package(SDL3 QUIET)

if(SDL3_FOUND)
    message(STATUS "Using system SDL3.")
else()
    message(STATUS "SDL3 not found, fetching...")

    set(SDL_TESTS OFF)

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(SDL3)
endif()

add_executable(chip8-emulator)
target_sources(chip8-emulator PRIVATE
    src/main.cpp
    src/Emulator.cpp
    src/Emulator.hpp
    src/CPU.cpp
    src/CPU.hpp
    src/Shared.hpp
    src/Font.hpp
)

target_compile_features(chip8-emulator PRIVATE cxx_std_23)
target_link_libraries(chip8-emulator PRIVATE SDL3::SDL3)

set_target_properties(chip8-emulator PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

if(WIN32)
    add_custom_command(TARGET chip8-emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:chip8-emulator> $<TARGET_RUNTIME_DLLS:chip8-emulator>
        COMMAND_EXPAND_LISTS
    )
endif()
