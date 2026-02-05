#include "CPU.hpp"

#include <format>
#include <fstream>
#include <vector>

CPU::CPU() {
    auto& s = m_state;
    for (size_t i = 0; i < s_font_size; ++i) {
        s.memory[i] = s_font_set[i];
    }
}

std::expected<void, std::string> CPU::load_rom(const std::filesystem::path& rom_path) {
    auto& s = m_state;
    std::string err_message;

    if (!std::filesystem::exists(rom_path)) {
        err_message = std::format("Failed to read the ROM file at the specified path: {}", rom_path.string());
        return std::unexpected(err_message);
    }

    auto rom_size = std::filesystem::file_size(rom_path);
    auto max_rom_size = s_memory_size - s_start_address;

    if (rom_size > max_rom_size) {
        err_message = std::format("The specified ROM file is too large. Size: {}", rom_size);
        return std::unexpected(err_message);
    }

    std::ifstream rom_file = std::ifstream(rom_path, std::ios::binary);

    if (!rom_file.is_open()) {
        err_message = "Failed to open the ROM file.";
        return std::unexpected(err_message);
    }

    std::vector<char> buffer(rom_size);
    rom_file.read(buffer.data(), rom_size);

    if (!rom_file) {
        err_message = "Failed to read the contents of the ROM file.";
        return std::unexpected(err_message);
    }

    for (size_t i = 0; i < rom_size; ++i) {
        s.memory[s_start_address + i] = buffer[i];
    }

    return {};
}
