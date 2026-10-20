#include <SDL2/SDL.h>



struct renderable_context
{
    SDL_Renderer * renderer;
    uint16_t window_height;

    SDL_FRect to_screen(SDL_FRect const & rectangle) const
    {
        return SDL_FRect{rectangle.x, static_cast<float>(window_height) - rectangle.y - rectangle.w, rectangle.w, rectangle.h};
    }

    SDL_FPoint to_screen(SDL_FPoint const & point) const
    {
        return SDL_FPoint{point.x, static_cast<float>(window_height) - point.y};
    }

};


class renderable
{
public:
    virtual ~renderable() = default;
    virtual void render(renderable_context const &) const = 0;
};
