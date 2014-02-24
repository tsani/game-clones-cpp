#ifndef FORSTATE_H
#define FORSTATE_H

#include <functional>

#include "State.h"

template<typename Iterator_t = int>
class ForState
    : public State
{
    public:
        ForState(const State *a_parent, Iterator_t a_start, Iterator_t a_end, std::function<void(Iterator_t)> a_updater, std::function<void()> a_callback)
            : State(a_parent), m_value(a_start), m_end(a_end), m_update(a_updater), m_callback(a_callback)
        {
        }

        virtual void update() override
        {
            if ( m_value != m_end )
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
                // cleanup sets the status to finished, which will cause the forstate to be cleaned up
                // by the parent.
                cleanup(); 
            }

            State::update();
        }

        // We expose setNext publicly so that the parent state can use the forstate to delay the entry
        // of another state, for example.
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
        std::function<void(Iterator_t)> m_update;
        std::function<void()> m_callback;

};

template<typename T = int>
std::shared_ptr<ForState<T>> 
makeDelay(const State *a_parent, int a_frames, std::function<void()> cb)
{
    return std::make_shared<ForState<T>>(a_parent, 0, a_frames, [] (int) {}, cb);
}

#endif
