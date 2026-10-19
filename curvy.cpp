#include <SDL_scancode.h>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "point.hpp"



int main()
{
    // Initialize SDL and create a window with renderer.
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error{std::string{"Unable to initialize SDL2:\n"} + SDL_GetError()};
    SDL_Window * window{SDL_CreateWindow
    (
        "curvy",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_HIDDEN
    )};
    if(window == nullptr)
        throw std::runtime_error{std::string{"Unable to create window:\n"} + SDL_GetError()};
    SDL_Renderer * renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if(renderer == nullptr)
        throw std::runtime_error{std::string{"Unable to create renderer"} + SDL_GetError()};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    // Create points and selection cursor.
    std::vector<point> points
    {
        glm::vec2{170.f, 260.f},
        glm::vec2{450.f, 650.f},
        glm::vec2{1100.f, 110.f}
    };
    std::vector<point>::iterator selected_point{points.begin()};

    // Animation start timestamp. If value is present, shows when the current animation was started by user. Otherwise
    // shows no animation currently running.
    std::optional<std::chrono::time_point<std::chrono::steady_clock>> animation_started_at;

    // Output control instructions.
    std::cout << "Tab to cycle through points; Shift+Tab to cycle in reverse.\n"
                 "Arrows to move selected point; Shift+Arrows to move fast; Ctrl+Arrows to move slow.\n"
                 "Space to show point ride animation." << std::endl;

    // Consume events and render as long as window is open.
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
            else if((event.type == SDL_KEYDOWN) and (event.key.repeat == 0))
            {
                auto mult = [](SDL_Keysym k) -> float
                {
                    if(k.mod == KMOD_LCTRL or k.mod == KMOD_RCTRL)
                        return 1.f;
                    if(k.mod == KMOD_LSHIFT or k.mod == KMOD_RSHIFT)
                        return 100.f;
                    return 10.f;
                };
                constexpr glm::vec2 x_axis{1.f, 0.f}, y_axis{0.f, 1.f};

                // Cycle cursor on tab key presses.
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

                // Move selected point on arrow key presses.
                else if(event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                    selected_point->increase_position(-x_axis * mult(event.key.keysym));
                else if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                    selected_point->increase_position(x_axis * mult(event.key.keysym));
                else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                    selected_point->increase_position(-y_axis * mult(event.key.keysym));
                else if(event.key.keysym.scancode == SDL_SCANCODE_UP)
                    selected_point->increase_position(y_axis * mult(event.key.keysym));

                // Start animation on space key presses.
                else if(event.key.keysym.scancode == SDL_SCANCODE_SPACE)
                    animation_started_at = std::chrono::steady_clock::now();
            }
        }

        // Exit without rendering if window should close.
        if(should_close)
            break;

        // Clear canvas with black background.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Render segments and points with white color.
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        for(auto a{points.begin()}; std::next(a) < points.end(); ++a)
            draw_line(renderer, 720, *a, *std::next(a));
        for(point & p : points)
            p.draw(renderer, 720, &(*selected_point) == &p);

        // Render animation if it was started.
        if(animation_started_at.has_value())
        {
            // Calculate inerpolation parameter and point indexes.
            auto difference = std::chrono::steady_clock::now() - animation_started_at.value();
            using duration_type = std::chrono::duration<float, std::ratio<1, 1>>;
            float const t{std::chrono::duration_cast<duration_type>(difference).count()};
            size_t const first{static_cast<size_t>(std::floor(t))},
                         second{static_cast<size_t>(std::ceil(t))};

            // If any are out of range, stop animation. Otherwise render on selected points.
            if(first >= points.size() or second >= points.size())
                animation_started_at.reset();
            else
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
                lerp(points[first], points[second], std::fmod(t, 1)).draw(renderer, 720, true);
            }
        }

        // Swap buffers.
        SDL_RenderPresent(renderer);
    }

    // Clean up.
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
