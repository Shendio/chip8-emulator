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

void CPU::step() {
    auto& s = m_state;

    s.opcode = (s.memory[s.pc] << 8) | s.memory[s.pc + 1];
    s.pc += 2;

    uint16_t type = (s.opcode & 0xF000) << 12;

    switch (type) {
    case 0x0000:
        op_0xxx();
        break;
    case 0x1000:
        op_1xxx();
        break;
    case 0x2000:
        op_2xxx();
        break;
    case 0x3000:
        op_3xxx();
        break;
    case 0x4000:
        op_4xxx();
        break;
    case 0x5000:
        op_5xxx();
        break;
    case 0x6000:
        op_6xxx();
        break;
    case 0x7000:
        op_7xxx();
        break;
    case 0x8000:
        op_8xxx();
        break;
    case 0x9000:
        op_9xxx();
        break;
    case 0xA000:
        op_Axxx();
        break;
    case 0xB000:
        op_Bxxx();
        break;
    case 0xC000:
        op_Cxxx();
        break;
    case 0xD000:
        op_Dxxx();
        break;
    case 0xE000:
        op_Exxx();
        break;
    case 0xF000:
        op_Fxxx();
        break;
    default:
        break;
    }
}

void CPU::op_noop() {
    // todo: properly handle illegal instructions
}

void CPU::op_0xxx() {
    auto& s = m_state;

    switch (s.opcode & 0xFF) {
    case 0xE0:
        s.display.fill(false);
        break;
    case 0xEE:
        s.pc = s.stack[s.sp];
        s.sp--;
        break;
    default:
        op_noop();
        break;
    }
}

void CPU::op_1xxx() {
    auto& s = m_state;

    uint16_t addr = (s.opcode & 0xFFF) << 4;
    s.pc = addr;
}

void CPU::op_2xxx() {
    auto& s = m_state;

    uint16_t addr = (s.opcode & 0xFFF) << 4;
    s.sp++;
    s.stack[s.sp] = s.pc;
    s.pc = addr;
}

void CPU::op_3xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t kk = s.opcode & 0x00FF;

    if (s.v[vx] == kk) {
        s.pc += 2;
    }
}
void op_4xxx();
void op_5xxx();
void op_6xxx();
void op_7xxx();
void op_8xxx();
void op_9xxx();
void op_Axxx();
void op_Bxxx();
void op_Cxxx();
void op_Dxxx();
void op_Exxx();
void op_Fxxx();
