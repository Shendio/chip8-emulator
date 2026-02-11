#pragma once

#include "CPU.hpp"

#include <array>
#include <expected>
#include <string>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Emulator {
public:
    Emulator() = default;
    ~Emulator();

    std::expected<void, std::string> init();
    void run();

private:
    void handle_events();
    void update();
    void render();

    CPU m_cpu;

    std::array<uint32_t, 64 * 32> m_pixels{};
    uint8_t m_ipf = 10;

    // SDL objects
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;

    // constants
    static constexpr int s_width = 1280;
    static constexpr int s_height = 640;
    static constexpr std::string_view s_title = "CHIP8 Emulator";

    bool m_running = false;
};
