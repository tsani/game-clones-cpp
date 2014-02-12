#ifndef STATE_H
#define STATE_H

#include <memory>

#include "util_SDL.h"

/** Defines the basic interface of all application states.
 */

// Child states must guarantee that their next state is loaded before finishing.
// The successor state should be in state notReady when its predecessor finishes.

class State;
typedef std::shared_ptr<State> State_ptr;

enum class AppState
{
    running,  // normal execution of a state
    paused,   // paused state's update method is generally just skipped
    finished, // state should be cleaned up
    notReady,
    ready
};

class State
{
    public:
        State (State *a_parent);

        virtual void update ();
        virtual void draw (Surface_ptr a_parent);
        virtual void load ();
        virtual void cleanup ();
        virtual void handleEvent (SDL_Event const& event);

        virtual void activate ();

    protected:
        virtual void setState (AppState a_state);
        virtual void setNext (State_ptr a_next); // a_next should have status ready.
        virtual void setChild (State_ptr a_child); // a_next should have status ready.

        State *m_parent;
        State_ptr m_child, m_next; // before becoming finished, this state must provide a value for next
        // as the parent state must replace its child with its child's next state.
        
        AppState m_status;
};

#endif
