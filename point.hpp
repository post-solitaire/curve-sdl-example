#include <SDL2/SDL.h>
#include <SDL_rect.h>
#include <SDL_render.h>



class point
{
    friend void draw_line(SDL_Renderer *, point const &, point const &);

public:
    point(float x, float y) : x{x}, y{y} {};
    ~point() = default;

    void draw(SDL_Renderer * renderer, bool is_selected) const
    {
        SDL_FRect point{x - point_size / 2.f, y - point_size / 2.f, point_size, point_size};
        SDL_RenderFillRectF(renderer, &point);
        if(is_selected)
        {
            SDL_FRect selection{x - selection_size / 2.f, y - selection_size / 2.f, selection_size, selection_size};
            SDL_RenderDrawRectF(renderer, &selection);
        }
    }

private:
    float x, y;

    static constexpr float point_size{5.f}, selection_size{20.f};
};



void draw_line(SDL_Renderer * renderer, point const & a, point const & b)
{
    SDL_RenderDrawLineF(renderer, a.x, a.y, b.x, b.y);
}
