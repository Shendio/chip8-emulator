#include "Emulator.hpp"

#include <charconv>
#include <print>
#include <span>
#include <string_view>
#include <system_error>

int main(int argc, char** argv) {
    Emulator emu;

    if (argc == 1) {
        std::println("Usage: ./Chappy8 [ --ipf <instruction_count> ] <path>");
        return 0;
    }

    auto args = std::span(argv, argc).subspan(1);

    std::string_view rom_path;
    uint8_t instruction_count = 10;

    for (size_t i = 0; i < args.size(); ++i) {
        std::string_view arg = args[i];

        if (arg == "--ipf" && i + 1 <= args.size()) {
            arg = args[++i];
            auto result = std::from_chars(arg.data(), arg.data() + arg.size(), instruction_count);
            if (result.ec != std::errc{}) {
                std::println("ERROR: Invalid argument for --ipf: {}", arg);
                return -1;
            }
            continue;
        }

        if (!arg.starts_with("--")) {
            rom_path = arg;
            continue;
        }

        std::println("ERROR: Invalid argument: {}", arg);
        return -1;
    }

    if (auto result = emu.init(rom_path, instruction_count); !result) {
        auto err_message = result.error();
        std::println("ERROR: {}", err_message);
        return -1;
    }

    emu.run();
}
