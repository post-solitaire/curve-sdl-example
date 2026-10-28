#pragma once

#include <cstdlib>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "renderable.hpp"



class point final : public renderable
{
public:
    point(glm::vec2 pos, bool selected = false)
            : position{pos}, is_selected{selected} {};
    ~point() = default;

    // Draw point, optionally draw selection.
    void render(renderable_context const & render_ctx) const override
    {
        render_ctx.fill_circle(position, point_size / 2.f, point_detail);
        if(is_selected)
            render_ctx.draw_circle(position, selection_size / 2.f, selection_detail);
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

    static constexpr float point_size{8.f}, selection_size{30.f};
    static constexpr uint8_t point_detail{8}, selection_detail{12};
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
