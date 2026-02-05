#include "Emulator.hpp"

#include <print>

int main(int argc, char** argv) {
    Emulator emu;

    if (auto result = emu.init(); !result) {
        auto err_message = result.error();
        std::println("ERROR: ", err_message);
        return -1;
    }

    emu.run();
}
