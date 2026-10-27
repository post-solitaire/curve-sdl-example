#pragma once

#include <SDL_render.h>
#include <cstdint>
#include <stdexcept>
#include <cmath>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/rotate_vector.hpp>



// Context POD for basic rendering via SDL API. Used to pass rendering context information to objects. Includes some
// basic helpers, primitive wrappers and fancy primitives.
struct renderable_context
{
    SDL_Renderer * renderer;
    uint16_t window_height;

    // Convert rectangle from normal space (bottom-left origin) to screen space (top-left origin).
    SDL_FRect to_screen(SDL_FRect const & rectangle) const
    {
        return SDL_FRect{rectangle.x, static_cast<float>(window_height) - rectangle.y - rectangle.h, rectangle.w, rectangle.h};
    }
    // Convert point from normal space (bottom-left origin) to screen space (top-left origin).
    SDL_FPoint to_screen(SDL_FPoint const & point) const
    {
        return SDL_FPoint{point.x, static_cast<float>(window_height) - point.y};
    }



    // Set draw color for primitives from a `glm::vec4` object, clamping color components between `0.f` and `1.f`.
    void set_draw_color(glm::vec4 color) const
    {
        glm::u8vec4 const norm_color{static_cast<glm::u8vec4>(255.f * glm::saturate(color))};
        if(SDL_SetRenderDrawColor(renderer, norm_color.r, norm_color.g, norm_color.b, norm_color.a) < 0)
            throw std::runtime_error{std::string{"Unable to set draw color:\n"} + SDL_GetError()};
    }
    // Draw line from `start` to `end` (in normal space).
    void draw_line(glm::vec2 start, glm::vec2 end) const
    {
        SDL_FPoint const a{to_screen(SDL_FPoint{start.x, start.y})}, b{to_screen(SDL_FPoint{end.x, end.y})};
        if(SDL_RenderDrawLineF(renderer, a.x, a.y, b.x, b.y) < 0)
            throw std::runtime_error{std::string{"Unable to draw line:\n"} + SDL_GetError()};
    }
    // Draw ruler line from `start` to `end` (in normal space). If line is shorter than `threshold`, draw simple line.
    // Otherwise it's drawn in 10 segments with 9 gaps of `gap` of total length between them at each tenth of length.
    void draw_ruler_line(glm::vec2 start, glm::vec2 end, float threshold = 100.f, float gap = .05f) const
    {
        if(glm::distance(start, end) < threshold)
        {
            draw_line(start, end);
            return;
        }
        gap /= 2.f;
        draw_line(start, glm::mix(start, end, .1f - gap));
        draw_line(glm::mix(start, end, .1f + gap), glm::mix(start, end, .2f - gap));
        draw_line(glm::mix(start, end, .2f + gap), glm::mix(start, end, .3f - gap));
        draw_line(glm::mix(start, end, .3f + gap), glm::mix(start, end, .4f - gap));
        draw_line(glm::mix(start, end, .4f + gap), glm::mix(start, end, .5f - gap));
        draw_line(glm::mix(start, end, .5f + gap), glm::mix(start, end, .6f - gap));
        draw_line(glm::mix(start, end, .6f + gap), glm::mix(start, end, .7f - gap));
        draw_line(glm::mix(start, end, .7f + gap), glm::mix(start, end, .8f - gap));
        draw_line(glm::mix(start, end, .8f + gap), glm::mix(start, end, .9f - gap));
        draw_line(glm::mix(start, end, .9f + gap), end);
    }
    // Draw circle from `center` with `radius` radius by drawing `detail` connected lines.
    void draw_circle(glm::vec2 center, float radius, uint8_t detail) const
    {
        for(uint8_t t{0}; t < detail; ++t)
        {
            float const p1{glm::two_pi<float>() * static_cast<float>(t) / static_cast<float>(detail)},
                        p2{glm::two_pi<float>() * static_cast<float>(t + 1) / static_cast<float>(detail)};
            constexpr glm::vec2 v_start{glm::vec2{1.f, 0.f}};
            draw_line(center + radius * glm::rotate(v_start, p1), center + radius * glm::rotate(v_start, p2));
        }
    }
};



class renderable
{
public:
    virtual ~renderable() = default;
    virtual void render(renderable_context const &) const = 0;
};
