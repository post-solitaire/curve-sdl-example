#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_video.h>
#include <SDL2/SDL.h>
#include <stdexcept>



int main()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        throw std::runtime_error{std::string{"Unable to initialize SDL2:\n"} + SDL_GetError()};
    SDL_Window * window;
    window = SDL_CreateWindow("curvy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 100, 100, 0);
    if(window == nullptr)
        throw std::runtime_error{std::string{"Unable to create window:\n"} + SDL_GetError()};

    bool should_close{false};
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
        }
        if(should_close)
            break;
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
