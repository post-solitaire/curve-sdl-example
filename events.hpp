#pragma once

#include <SDL2/SDL.h>



class event
{
public:
    enum class consume_mode
    {
        // The SDL event was missed completely, usually due to incompatible SDL event type, incompatible SDL event data,
        // disabled event or a missing action callable.
        miss,

        // The SDL event was consumed and the action was performed, but the event doesn't care if other events get
        // trigger by the same SDL event.
        fallthrough,

        // The SDL event was consumed and the action was performed, but the event doesn't allow other events to consume
        // that SDL event.
        stop,
    };

    virtual ~event() = default;
    virtual consume_mode handle(SDL_Event const &) const = 0;
};
