#include "Emulator.hpp"

#include <SDL3/SDL.h>

Emulator::~Emulator() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

std::expected<void, std::string> Emulator::init() {
    std::string error_message;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        error_message = SDL_GetError();
        return std::unexpected(error_message);
    }

    if (!SDL_CreateWindowAndRenderer(s_title.data(), s_width, s_height, 0, &m_window, &m_renderer)) {
        error_message = SDL_GetError();
        return std::unexpected(error_message);
    }

    m_running = true;
    return {};
}

void Emulator::run() {
    while (m_running) {
        handle_events();
        update();
        render();
    }
}

void Emulator::handle_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_QUIT:
            m_running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (e.key.scancode == SDL_SCANCODE_ESCAPE) {
                m_running = false;
                break;
            }
            break;
        default:
            break;
        }
    }
}

void Emulator::update() {
    // todo: run the emulator here
}

void Emulator::render() {
    // todo: render the emulator here
}
