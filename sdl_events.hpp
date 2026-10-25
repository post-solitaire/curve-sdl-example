#pragma once

#include <SDL_events.h>
#include <SDL_scancode.h>
#include <functional>

#include "events.hpp"



class quit_event final : public event
{
public:
    using action_type = std::function<void()>;

    quit_event(action_type const & action = {}) : action{action} {}
    virtual ~quit_event() = default;

    consume_mode handle(SDL_Event const & event) const override
    {
        if((event.type != SDL_QUIT) or (not action))
            return consume_mode::miss;
        action();
        return consume_mode::fallthrough;
    }

    void set_action(action_type const & new_action = {}) { action = new_action; }
    action_type get_action() const { return action; }

private:
    action_type action;
};



class key_event final : public event
{
public:
    using action_type = std::function<void(SDL_Keysym)>;
    using act_cref = action_type const &;

    key_event(SDL_Scancode k, act_cref press = {}, act_cref release = {}, act_cref repeat = {})
            : key{k}, press_action{press}, release_action{release}, repeat_action{repeat} {}
    virtual ~key_event() = default;

    consume_mode handle(SDL_Event const & event) const override
    {
        if((event.type != SDL_KEYDOWN) and (event.type != SDL_KEYUP))
            return consume_mode::miss;
        if(event.key.keysym.scancode != key)
            return consume_mode::miss;
        if(event.type == SDL_KEYUP)
        {
            if(not release_action)
                return consume_mode::miss;
            release_action(event.key.keysym);
        }
        else if(event.type == SDL_KEYDOWN)
        {
            if(event.key.repeat == 0)
            {
                if(not press_action)
                    return consume_mode::miss;
                press_action(event.key.keysym);
            }
            else
            {
                if(not repeat_action)
                    return consume_mode::miss;
                repeat_action(event.key.keysym);
            }
        }
        return consume_mode::fallthrough;
    }

    void set_press_action(act_cref new_action = {}) { press_action = new_action; }
    void set_release_action(act_cref new_action = {}) { release_action = new_action; }
    void set_repeat_action(act_cref new_action = {}) { repeat_action = new_action; }
    void set_key(SDL_Scancode new_key) { key = new_key; }
    action_type get_press_action() const { return press_action; }
    action_type get_release_action() const { return release_action; }
    action_type get_repeat_action() const { return repeat_action; }
    SDL_Scancode get_key() const { return key; }

private:
    SDL_Scancode key;
    action_type press_action, release_action, repeat_action;
};
