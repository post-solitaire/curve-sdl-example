#include <cstdlib>

#include <SDL2/SDL.h>
#include <glm/glm.hpp>



class point
{
public:
    point(glm::vec2 position) : position{position} {};
    ~point() = default;

    // Draw point (assuming bottom-left origin), optionally draw selection box.
    void draw(SDL_Renderer * renderer, uint16_t window_height, bool is_selected) const
    {
        SDL_FRect point
        {
            position.x - point_half_size, static_cast<float>(window_height) - (position.y - point_half_size),
            point_size, -point_size
        };
        SDL_RenderFillRectF(renderer, &point);
        if(is_selected)
        {
            SDL_FRect selection
            {
                position.x - selection_half_size, static_cast<float>(window_height) - (position.y - selection_half_size),
                selection_size, -selection_size
            };
            SDL_RenderDrawRectF(renderer, &selection);
        }
    }

    // Position getters and setters.
    void set_position(glm::vec2 new_position) { position = new_position; }
    glm::vec2 get_position() const { return position; }
    glm::vec2 increase_position(glm::vec2 delta) { return position += delta; }

private:
    glm::vec2 position;

    static constexpr float point_size{5.f}, selection_size{20.f}, point_half_size{point_size / 2.f},
                           selection_half_size{selection_size / 2.f};
};



// Draw segment between two points.
void draw_line(SDL_Renderer * renderer, uint16_t window_height, point const & a, point const & b)
{
    SDL_RenderDrawLineF
    (
        renderer,
        a.get_position().x, static_cast<float>(window_height) - a.get_position().y,
        b.get_position().x, static_cast<float>(window_height) - b.get_position().y
    );
}

point lerp(point const & a, point const & b, float t)
{
    return point{glm::mix(a.get_position(), b.get_position(), t)};
}
