#include <SDL_keyboard.h>
#include <SDL_keycode.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <iterator>
#include <stdexcept>
#include <iostream>

#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_pixels.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL2/SDL.h>
#include <vector>

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

    std::vector<point> points{{20.f, 20.f}, {50.f, 50.f}, {100.f, 10.f}};
    std::vector<point>::iterator selected_point{points.begin()};

    // Consume events as long as window is open.
    bool should_close{false};
    SDL_ShowWindow(window);
    std::cout << "Tab to cycle through points; Shift+Tab to cycle in reverse.\n"
                 "Arrows to move selected point; Shift+Arrows to move fast; Ctrl+Arrows to move slow." << std::endl;
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
            else if((event.type == SDL_KEYDOWN) and (event.key.repeat == 0))
            {
                if(event.key.keysym.scancode == SDL_SCANCODE_TAB)
                {
                    if(event.key.keysym.mod == KMOD_LSHIFT or event.key.keysym.mod == KMOD_RSHIFT)
                    {
                        --selected_point;
                        if(selected_point < points.begin())
                            selected_point = std::prev(points.end());
                    }
                    else
                    {
                        ++selected_point;
                        if(selected_point >= points.end())
                            selected_point = points.begin();
                    }
                }
                auto mult = [](SDL_Keysym k) -> float
                {
                    if(k.mod == KMOD_LCTRL or k.mod == KMOD_RCTRL)
                        return 1.f;
                    if(k.mod == KMOD_LSHIFT or k.mod == KMOD_RSHIFT)
                        return 100.f;
                    return 10.f;
                };
                if(event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                    selected_point->increase_x(-mult(event.key.keysym));
                if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                    selected_point->increase_x(mult(event.key.keysym));
                if(event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                    selected_point->increase_y(-mult(event.key.keysym));
                if(event.key.keysym.scancode == SDL_SCANCODE_UP)
                    selected_point->increase_y(mult(event.key.keysym));
            }
        }

        if(should_close)
            break;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        for(auto a{points.begin()}; std::next(a) < points.end(); ++a)
            draw_line(renderer, 720, *a, *std::next(a));
        for(point & p : points)
            p.draw(renderer, 720, &(*selected_point) == &p);

        SDL_RenderPresent(renderer);
    }

    // Clean up.
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
