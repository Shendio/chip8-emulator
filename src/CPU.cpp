#include "CPU.hpp"
#include "Font.hpp"

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

    s.status = CPUStatus::RUNNING;

    return {};
}

void CPU::step() {
    auto& s = m_state;

    s.opcode = (s.memory[s.pc] << 8) | s.memory[s.pc + 1];
    s.pc += 2;

    uint8_t type = (s.opcode & 0xF000) >> 12;

    switch (type) {
    case 0x0:
        op_0xxx();
        break;
    case 0x1:
        op_1xxx();
        break;
    case 0x2:
        op_2xxx();
        break;
    case 0x3:
        op_3xxx();
        break;
    case 0x4:
        op_4xxx();
        break;
    case 0x5:
        op_5xxx();
        break;
    case 0x6:
        op_6xxx();
        break;
    case 0x7:
        op_7xxx();
        break;
    case 0x8:
        op_8xxx();
        break;
    case 0x9:
        op_9xxx();
        break;
    case 0xA:
        op_Axxx();
        break;
    case 0xB:
        op_Bxxx();
        break;
    case 0xC:
        op_Cxxx();
        break;
    case 0xD:
        op_Dxxx();
        break;
    case 0xE:
        op_Exxx();
        break;
    case 0xF:
        op_Fxxx();
        break;
    default:
        op_noop();
        break;
    }
}

void CPU::update_timers() {
    auto& s = m_state;
    if (s.delay_timer) {
        s.delay_timer--;
    }
    if (s.sound_timer) {
        s.sound_timer--;
    }
}

void CPU::op_noop() {
    auto& s = m_state;
    s.status = CPUStatus::CRASHED;
}

void CPU::op_0xxx() {
    auto& s = m_state;

    switch (s.opcode & 0xFF) {
    case 0xE0:
        s.display.fill(0);
        m_draw_flag = true;
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

    uint16_t addr = s.opcode & 0xFFF;
    s.pc = addr;
}

void CPU::op_2xxx() {
    auto& s = m_state;

    uint16_t addr = s.opcode & 0xFFF;
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

void CPU::op_4xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t kk = s.opcode & 0xFF;

    if (s.v[vx] != kk) {
        s.pc += 2;
    }
}

void CPU::op_5xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t vy = get_y();

    if (s.v[vx] == s.v[vy]) {
        s.pc += 2;
    }
}

void CPU::op_6xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t kk = s.opcode & 0xFF;

    s.v[vx] = kk;
}

void CPU::op_7xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t kk = s.opcode & 0xFF;

    s.v[vx] = s.v[vx] + kk;
}

void CPU::op_8xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t vy = get_y();

    switch (s.opcode & 0xF) {
    case 0x0:
        s.v[vx] = s.v[vy];
        break;
    case 0x1:
        s.v[vx] = s.v[vx] | s.v[vy];
        break;
    case 0x2:
        s.v[vx] = s.v[vx] & s.v[vy];
        break;
    case 0x3:
        s.v[vx] = s.v[vx] ^ s.v[vy];
        break;
    case 0x4: {
        uint16_t sum = s.v[vx] + s.v[vy];
        bool carry = sum > 255;
        s.v[vx] = static_cast<uint8_t>(sum);
        s.v[0xF] = carry ? 1 : 0;
        break;
    }
    case 0x5: {
        bool not_borrow = s.v[vx] >= s.v[vy];
        s.v[vx] = s.v[vx] - s.v[vy];
        s.v[0xF] = not_borrow;
        break;
    }
    case 0x6: {
        bool lsb = s.v[vx] & 0x1;
        s.v[vx] /= 2;
        s.v[0xF] = lsb;
        break;
    }
    case 0x7: {
        bool not_borrow = s.v[vy] >= s.v[vx];
        s.v[vx] = s.v[vy] - s.v[vx];
        s.v[0xF] = not_borrow;
        break;
    }
    case 0xE: {
        bool msb = s.v[vx] & 0x80;
        s.v[vx] *= 2;
        s.v[0xF] = msb;
        break;
    }
    default:
        op_noop();
        break;
    }
}

void CPU::op_9xxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t vy = get_y();

    if (s.v[vx] != s.v[vy]) {
        s.pc += 2;
    }
}

void CPU::op_Axxx() {
    auto& s = m_state;

    uint16_t addr = s.opcode & 0xFFF;

    s.i = addr;
}

void CPU::op_Bxxx() {
    auto& s = m_state;

    uint16_t addr = s.opcode & 0xFFF;

    s.pc = addr + s.v[0x0];
}

void CPU::op_Cxxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t kk = s.opcode & 0xFF;
    uint8_t random_number = static_cast<uint8_t>(m_rng_dist(m_rng_gen));

    s.v[vx] = random_number & kk;
}

void CPU::op_Dxxx() {
    auto& s = m_state;

    uint8_t vx = get_x();
    uint8_t vy = get_y();
    uint8_t height = s.opcode & 0xF;

    uint8_t pos_x = s.v[vx] % s_display_width;
    uint8_t pos_y = s.v[vy] % s_display_height;
    s.v[0xF] = 0;

    for (size_t row = 0; row < height; ++row) {
        uint8_t sprite_byte = s.memory[s.i + row];
        for (size_t col = 0; col < 8; ++col) {
            bool sprite_pixel = sprite_byte & (0x80 >> col);
            if (sprite_pixel) {
                uint8_t screen_x = pos_x + col;
                uint8_t screen_y = pos_y + row;

                if (screen_x >= 64 || screen_y >= 32) {
                    continue;
                }

                uint16_t pixel_index = s_display_width * screen_y + screen_x;

                if (s.display[pixel_index]) {
                    s.v[0xF] = 1;
                }

                s.display[pixel_index] ^= 1;
            }
        }
    }
    m_draw_flag = true;
}

void CPU::op_Exxx() {
    auto& s = m_state;

    uint8_t vx = get_x();

    switch (s.opcode & 0xFF) {
    case 0x9E:
        if (s.key_state[s.v[vx] & 0xF]) {
            s.pc += 2;
        }
        break;
    case 0xA1:
        if (!s.key_state[s.v[vx] & 0xF]) {
            s.pc += 2;
        }
        break;
    default:
        op_noop();
        break;
    }
}

void CPU::op_Fxxx() {
    auto& s = m_state;

    uint8_t vx = get_x();

    switch (s.opcode & 0xFF) {
    case 0x7:
        s.v[vx] = s.delay_timer;
        break;
    case 0xA: {
        bool pressed = false;
        for (size_t i = 0; i < 16; ++i) {
            if (s.key_state[i]) {
                s.v[vx] = i;
                pressed = true;
                break;
            }
        }
        if (!pressed) {
            s.pc -= 2;
        }
        break;
    }
    case 0x15:
        s.delay_timer = s.v[vx];
        break;
    case 0x18:
        s.sound_timer = s.v[vx];
        break;
    case 0x1E:
        s.i = s.i + s.v[vx];
        break;
    case 0x29: {
        uint8_t digit = s.v[vx];
        s.i = static_cast<uint8_t>(5 * digit);
        break;
    }
    case 0x33: {
        s.memory[s.i] = s.v[vx] / 100;
        s.memory[s.i + 1] = (s.v[vx] / 10) % 10;
        s.memory[s.i + 2] = s.v[vx] % 10;
        break;
    }
    case 0x55: {
        for (size_t index = 0; index <= vx; ++index) {
            s.memory[s.i + index] = s.v[index];
        }
        break;
    }
    case 0x65: {
        for (size_t index = 0; index <= vx; ++index) {
            s.v[index] = s.memory[s.i + index];
        }
        break;
    }
    default:
        op_noop();
        break;
    }
}
