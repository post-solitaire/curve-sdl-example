#include <SDL2/SDL.h>
#include <SDL_events.h>
#include <SDL_keyboard.h>
#include <functional>
#include <stdexcept>



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








class quit_event final : public event
{
public:
    using action_type = std::function<void()>;

    quit_event(action_type const & action) : action{action}
    {
        if(not action)
            throw std::runtime_error{"Event action cannot be empty."};
    }
    virtual ~quit_event() = default;

    consume_mode handle(SDL_Event const & event) const override
    {
        if((event.type != SDL_QUIT) or (not action))
            return consume_mode::miss;
        action();
        return consume_mode::fallthrough;
    }

    void set_action(action_type const & new_action)
    {
        if(not new_action)
            throw std::runtime_error{"Event action cannot be empty."};
        action = new_action;
    }
    action_type get_action() const { return action; }

private:
    action_type action;
};
