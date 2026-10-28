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
#include "sdl_events.hpp"



int main()
{
    // Initialize SDL and create a window.
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

    // Create a renderer and renderable context.
    SDL_Renderer * renderer{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    if(renderer == nullptr)
        throw std::runtime_error{std::string{"Unable to create renderer"} + SDL_GetError()};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    renderable_context const render_ctx{renderer, 720};



    // Create points and selection cursor. Create key action to cycle through points.
    std::vector<point> points
    {
        glm::vec2{170.f, 260.f},
        glm::vec2{450.f, 650.f},
        glm::vec2{1100.f, 110.f}
    };
    std::vector<point>::iterator selected_point{points.begin()};
    selected_point->set_is_selected(true);
    key_handler const cycle
    {
        SDL_SCANCODE_TAB,
        [&](SDL_Keysym k)
        {
            selected_point->set_is_selected(false);
            if(k.mod == KMOD_LSHIFT or k.mod == KMOD_RSHIFT)
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
            selected_point->set_is_selected(true);
        }
    };

    // Create animation start timestamp and key action to start animation. If value is present, shows when the current
    // animation was started by user. Otherwise shows no animation currently running.
    std::optional<std::chrono::time_point<std::chrono::steady_clock>> animation_started_at;
    key_handler const start_animation
    {
        SDL_SCANCODE_SPACE,
        [&](SDL_Keysym){ animation_started_at = std::chrono::steady_clock::now(); }
    };

    // Create quit event with outside boolean flag.
    bool should_close{false};
    quit_handler const quit{[&](){ should_close = true; }};

    // Create point movement actions.
    auto mult = [](SDL_Keysym k) -> float
    {
        if(k.mod == KMOD_LCTRL or k.mod == KMOD_RCTRL)
            return 1.f;
        if(k.mod == KMOD_LSHIFT or k.mod == KMOD_RSHIFT)
            return 100.f;
        return 10.f;
    };
    constexpr glm::vec2 x_axis{1.f, 0.f}, y_axis{0.f, 1.f};
    key_handler const move_left
    {
        SDL_SCANCODE_LEFT,
        [&](SDL_Keysym k){ selected_point->increase_position(-x_axis * mult(k)); }
    };
    key_handler const move_right
    {
        SDL_SCANCODE_RIGHT,
        [&](SDL_Keysym k){ selected_point->increase_position(x_axis * mult(k)); }
    };
    key_handler const move_down
    {
        SDL_SCANCODE_DOWN,
        [&](SDL_Keysym k){ selected_point->increase_position(-y_axis * mult(k)); }
    };
    key_handler const move_up
    {
        SDL_SCANCODE_UP,
        [&](SDL_Keysym k){ selected_point->increase_position(y_axis * mult(k)); }
    };

    // Put all actions into one place to iterate on.
    std::vector<handler const *> events{&quit, &cycle, &start_animation, &move_left, &move_right, &move_down, &move_up};



    // Output control instructions to terminal.
    std::cout << "Tab to cycle through points; Shift+Tab to cycle in reverse.\n"
                 "Arrows to move selected point; Shift+Arrows to move fast; Ctrl+Arrows to move slow.\n"
                 "Space to show point ride animation." << std::endl;

    // Consume events and render as long as window is open.
    SDL_ShowWindow(window);
    while(not should_close)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event) > 0)
        {
            for(auto const e : events)
                if(e->handle(event) == handler::consume_mode::stop)
                    break;
        }

        // Exit without rendering if window should close.
        if(should_close)
            break;

        // Clear canvas with black background.
        render_ctx.set_draw_color(glm::vec4{.1f, .1f, .1f, 0.f});
        SDL_RenderClear(renderer);

        // Render segments and points with white color.
        render_ctx.set_draw_color(glm::vec4{1.f, 1.f, 1.f, 0.f});
        for(auto a{points.begin()}; std::next(a) < points.end(); ++a)
            render_line(render_ctx, *a, *std::next(a));
        for(point & p : points)
            p.render(render_ctx);

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
                render_ctx.set_draw_color(glm::vec4{1.f, 0.f, 1.f, 0.f});
                point p{lerp(points[first], points[second], static_cast<float>(std::fmod(t, 1)))};
                p.set_is_selected(true);
                p.render(render_ctx);
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
