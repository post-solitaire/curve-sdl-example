#include <stdexcept>

#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL2/SDL.h>

#include "point.hpp"



int main()
{
    // Initialize SDL and create a window.
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error{std::string{"Unable to initialize SDL2:\n"} + SDL_GetError()};
    SDL_Window * window;
    SDL_Renderer * renderer;
    window = SDL_CreateWindow("curvy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_HIDDEN);
    if(window == nullptr)
        throw std::runtime_error{std::string{"Unable to create window:\n"} + SDL_GetError()};
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(renderer == nullptr)
        throw std::runtime_error{std::string{"Unable to create renderer"} + SDL_GetError()};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    point a{20.f, 20.f}, b{50.f, 50.f};

    // Consume events as long as window is open.
    bool should_close{false};
    SDL_ShowWindow(window);
    while(not should_close)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event) > 0)
        {
            if(event.type == SDL_QUIT)
            {
                should_close = true;
                break;
            }
        }
        if(should_close)
            break;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        draw_line(renderer, a, b);
        a.draw(renderer, false);
        b.draw(renderer, true);

        SDL_RenderPresent(renderer);
    }

    // Clean up.
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
