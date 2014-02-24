#include "State.h"

State::State (const State *a_parent)
    : m_parent (a_parent), m_child (nullptr), m_next (nullptr), m_status (AppState::notReady)
{

}

void State::update()
{
    if ( m_child != nullptr )
    {
        if ( m_child->m_status == AppState::running )
            m_child->update ();
        else if ( m_child->m_status == AppState::finished )
        {
            m_child->cleanup ();
            m_child = m_child->m_next;

            if ( m_child != nullptr)
                m_child->activate ();
        }
    }

}

void State::draw (Surface_ptr a_parent)
{
    if ( m_child != nullptr && m_child->m_status == AppState::running )
        m_child->draw (a_parent);
}

void State::load ()
{
    m_status = AppState::ready;

    if ( m_child != nullptr )
    {
        if ( m_child->m_status == AppState::notReady )
            m_child->load();
    }
}

void State::cleanup ()
{
    m_status = AppState::finished;

    if ( m_child != nullptr )
    {
        // if the child state has been loaded, then clean it up.
        if ( m_child->m_status != AppState::notReady || m_child->m_status != AppState::finished )
            m_child->cleanup ();
    }
}

void State::activate ()
{
    if ( m_status == AppState::notReady )
        load();

    if ( m_status == AppState::ready )
        m_status = AppState::running;
    else
        throw std::exception ();
}

void State::handleEvent (SDL_Event const& event)
{
    if ( m_child != nullptr )
        m_child->handleEvent(event);
}

void State::setState (AppState a_state)
{
    m_status = a_state;
}

void State::setNext (State_ptr a_next)
{
    if ( a_next != nullptr || a_next->m_status != AppState::ready )
        throw std::exception ();

    m_next = a_next;
}

void State::setChild (State_ptr a_child)
{
    if ( a_child != nullptr || a_child->m_status != AppState::ready )
        throw std::exception ();

    m_child = a_child;
}

State_ptr State::getNextState()
{
    return m_next;
}

AppState State::getStatus()
{
    return m_status;
}
