#pragma once

#include "CPU.hpp"
#include "Shared.hpp"

#include <array>
#include <expected>
#include <string>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class Emulator {
private:
    static constexpr int s_width = 1280;
    static constexpr int s_height = 640;
    static constexpr std::string_view s_title = "CHIP8 Emulator";

public:
    Emulator() = default;
    ~Emulator();

    std::expected<void, std::string> init(const std::filesystem::path& rom_path, uint8_t ipf);
    void run();

private:
    void handle_events();
    void update();
    void render();

    CPU m_cpu;

    std::array<uint32_t, s_display_width * s_display_height> m_pixels{};
    uint8_t m_ipf = s_default_ipf;

    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture* m_texture;
};
