#include <SDL2/SDL.h>
#include <SDL_rect.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <cstdlib>



class point
{
public:
    point(float x, float y) : x{x}, y{y} {};
    ~point() = default;

    void draw(SDL_Renderer * renderer, uint16_t window_height, bool is_selected) const
    {
        SDL_FRect point
        {
            x - point_half_size, static_cast<float>(window_height) - (y - point_half_size),
            point_size, -point_size
        };
        SDL_RenderFillRectF(renderer, &point);
        if(is_selected)
        {
            SDL_FRect selection
            {
                x - selection_half_size, static_cast<float>(window_height) - (y - selection_half_size),
                selection_size, -selection_size
            };
            SDL_RenderDrawRectF(renderer, &selection);
        }
    }

    void set_x(float x) { this->x = x; }
    void set_y(float y) { this->y = y; }
    float get_x() const { return x; }
    float get_y() const { return y; }
    float increase_x(float delta) { return (this->x += delta); }
    float increase_y(float delta) { return (this->y += delta); }

private:
    float x, y;

    static constexpr float point_size{5.f}, selection_size{20.f}, point_half_size{point_size / 2.f},
                           selection_half_size{selection_size / 2.f};
};



void draw_line(SDL_Renderer * renderer, uint16_t window_height, point const & a, point const & b)
{
    SDL_RenderDrawLineF
    (
        renderer,
        a.get_x(), static_cast<float>(window_height) - a.get_y(),
        b.get_x(), static_cast<float>(window_height) - b.get_y()
    );
}
