#pragma once

#include <cstdlib>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "renderable.hpp"



class point final : public renderable
{
public:
    point(glm::vec2 position, bool is_selected = false)
            : position{position}, is_selected{is_selected} {};
    ~point() = default;

    // Draw point (assuming bottom-left origin), optionally draw selection box.
    void render(renderable_context const & render_ctx) const override
    {
        SDL_FRect const point{render_ctx.to_screen(SDL_FRect{position.x - point_half_size, position.y - point_half_size, point_size, point_size})};
        SDL_RenderFillRectF(render_ctx.renderer, &point);
        if(is_selected)
            render_ctx.draw_circle(position, selection_size / 2.f, 12);
    }

    // Position getters and setters.
    void set_position(glm::vec2 new_position) { position = new_position; }
    glm::vec2 get_position() const { return position; }
    glm::vec2 increase_position(glm::vec2 delta) { return position += delta; }

    // Selection getter and setter.
    void set_is_selected(bool new_is_selected) { is_selected = new_is_selected; }
    bool get_is_selected() const { return is_selected; }

    explicit operator SDL_FPoint() const
    {
        return SDL_FPoint{position.x, position.y};
    }

private:
    glm::vec2 position;
    bool is_selected;

    static constexpr float point_size{5.f}, selection_size{20.f}, point_half_size{point_size / 2.f};
};




// Draw segment between two points.
inline void render_line(renderable_context const & render_ctx, point const & a, point const & b)
{
    render_ctx.draw_ruler_line(a.get_position(), b.get_position());
}

inline point lerp(point const & a, point const & b, float t)
{
    return point{glm::mix(a.get_position(), b.get_position(), t)};
}
