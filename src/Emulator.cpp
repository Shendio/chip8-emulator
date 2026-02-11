#include "Emulator.hpp"

#include <chrono>
#include <optional>
#include <thread>

#include <SDL3/SDL.h>

static std::optional<uint8_t> map_key_to_chip8(SDL_Scancode sc) {
    switch (sc) {
    case SDL_SCANCODE_1:
        return 0x1;
    case SDL_SCANCODE_2:
        return 0x2;
    case SDL_SCANCODE_3:
        return 0x3;
    case SDL_SCANCODE_4:
        return 0xC;
    case SDL_SCANCODE_Q:
        return 0x4;
    case SDL_SCANCODE_W:
        return 0x5;
    case SDL_SCANCODE_E:
        return 0x6;
    case SDL_SCANCODE_R:
        return 0xD;
    case SDL_SCANCODE_A:
        return 0x7;
    case SDL_SCANCODE_S:
        return 0x8;
    case SDL_SCANCODE_D:
        return 0x9;
    case SDL_SCANCODE_F:
        return 0xE;
    case SDL_SCANCODE_Z:
        return 0xA;
    case SDL_SCANCODE_X:
        return 0x0;
    case SDL_SCANCODE_C:
        return 0xB;
    case SDL_SCANCODE_V:
        return 0xF;
    default:
        return std::nullopt;
    }
}

Emulator::~Emulator() {
    SDL_DestroyTexture(m_texture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

std::expected<void, std::string> Emulator::init(const std::filesystem::path& rom_path, uint8_t ipf) {
    std::string err_message;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        err_message = SDL_GetError();
        return std::unexpected(err_message);
    }

    if (!SDL_CreateWindowAndRenderer(s_title.data(), s_width, s_height, 0, &m_window, &m_renderer)) {
        err_message = SDL_GetError();
        return std::unexpected(err_message);
    }

    if (m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 64, 32); !m_texture) {
        err_message = SDL_GetError();
        return std::unexpected(err_message);
    }

    if (ipf == 0) {
        err_message = "IPF variable cannot be set to zero.";
        return std::unexpected(err_message);
    }

    m_ipf = ipf;

    SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_NEAREST);

    if (auto result = m_cpu.load_rom(rom_path); !result) {
        err_message = result.error();
        return std::unexpected(err_message);
    }

    m_running = true;
    return {};
}

void Emulator::run() {
    using namespace std::chrono_literals;
    constexpr auto desired_frame_time = 16.67ms;
    while (m_running) {
        const auto frame_start = std::chrono::high_resolution_clock::now();

        handle_events();
        update();
        render();

        const auto frame_time = std::chrono::high_resolution_clock::now() - frame_start;
        if (desired_frame_time > frame_time) {
            std::this_thread::sleep_for(desired_frame_time - frame_time);
        }
    }
}

void Emulator::handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_QUIT:
            m_running = false;
            break;
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_KEY_DOWN: {
            if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
                m_running = false;
                break;
            }

            auto key_pressed = map_key_to_chip8(e.key.scancode);
            if (key_pressed.has_value()) {
                bool pressed = (e.type == SDL_EVENT_KEY_DOWN);
                m_cpu.set_key_state(key_pressed.value(), pressed);
            }

            break;
        }
        default:
            break;
        }
    }
}

void Emulator::update() {
    for (size_t i = 0; i < m_ipf; ++i) {
        m_cpu.step();
    }

    m_cpu.update_timers();
}

void Emulator::render() {
    if (m_cpu.get_draw_flag()) {
        m_cpu.reset_draw_flag();

        auto& display_ref = m_cpu.get_display_data();

        for (size_t i = 0; i < 64 * 32; ++i) {
            m_pixels[i] = display_ref[i] ? 0xFFFFFFFF : 0xFF000000;
        }

        SDL_UpdateTexture(m_texture, nullptr, m_pixels.data(), 64 * sizeof(uint32_t));
        SDL_RenderClear(m_renderer);
        SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
        SDL_RenderPresent(m_renderer);
    }
}
