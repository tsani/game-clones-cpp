#ifndef FORSTATE_H
#define FORSTATE_H

#include "State.h"

template<typename Update_t, typename Callback_t>
class ForState
    : public State
{
    public:
        ForState(const State *a_parent, int a_start, int a_end, Update_t a_update, Callback_t a_callback)
            : State(a_parent), m_value(a_start), m_end(a_end), m_update(a_update), m_callback(a_callback)
        {
        }

        virtual void update() override
        {
            if ( m_value < m_end )
            {
                m_update(m_value);
                m_value++;
            }
            else
            {
                // NOTE
                // Do we maybe want to move the callback into the cleanup method?
                // If we do, and this object gets cleaned up prematurely, then the callback gets called...
                // not sure if this is desirable.
                m_callback();
                cleanup();
            }

            State::update();
        }

        void setNext(State_ptr next)
        {
            m_next = next;
        }

        int getValue()
        {
            return m_value;
        }

    private:
        int m_value, m_end;
        Update_t m_update;
        Callback_t m_callback;

};

static constexpr auto empty = [] (int) {};

template<class Callback>
std::shared_ptr<ForState<decltype(empty), Callback>> 
makeDelay(const State *a_parent, int a_frames, Callback & cb)
{
    return std::make_shared<ForState<decltype(empty), Callback>>(a_parent, 0, a_frames, empty, cb);
}

#endif
