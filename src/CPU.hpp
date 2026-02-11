#pragma once

#include "Shared.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <random>
#include <string>

enum class CPUStatus : uint8_t { UNINITIALIZED, RUNNING, CRASHED, STOPPED };

class CPU {
private:
    static constexpr size_t s_memory_size = 4096;
    static constexpr size_t s_start_address = 0x200;

public:
    CPU();

    std::expected<void, std::string> load_rom(const std::filesystem::path& rom_path);
    void step();
    void update_timers();

    void set_status(CPUStatus status) { m_state.status = status; }
    CPUStatus get_status() const { return m_state.status; }
    void set_key_state(size_t index, bool state) { m_state.key_state[index] = state; }
    bool get_draw_flag() const { return m_draw_flag; }
    void reset_draw_flag() { m_draw_flag = 0; }
    const std::array<uint8_t, s_display_width * s_display_height>& get_display_data() { return m_state.display; }

private:
    struct State {
        std::array<uint8_t, s_memory_size> memory{};
        std::array<uint8_t, s_display_width * s_display_height> display{};
        std::array<uint16_t, 16> stack{};
        std::array<uint8_t, 16> v{};
        std::array<bool, 16> key_state{};
        uint16_t pc = s_start_address;
        uint16_t opcode{};
        uint16_t i{};
        uint8_t sp{};
        uint8_t delay_timer{};
        uint8_t sound_timer{};
        CPUStatus status = CPUStatus::UNINITIALIZED;
    };

    State m_state;
    bool m_draw_flag = false;

    std::random_device rd{};
    std::mt19937 m_rng_gen{rd()};
    std::uniform_int_distribution<uint16_t> m_rng_dist{};

    uint8_t get_x() const { return (m_state.opcode & 0x0F00) >> 8; }
    uint8_t get_y() const { return (m_state.opcode & 0x00F0) >> 4; }

    void op_noop();
    // clang-format off
    void op_0xxx(); void op_1xxx(); void op_2xxx(); void op_3xxx();
    void op_4xxx(); void op_5xxx(); void op_6xxx(); void op_7xxx();
    void op_8xxx(); void op_9xxx(); void op_Axxx(); void op_Bxxx();
    void op_Cxxx(); void op_Dxxx(); void op_Exxx(); void op_Fxxx();
    // clang-format on
};
