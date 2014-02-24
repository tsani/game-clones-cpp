#ifndef MULTISTATE_H
#define MULTISTATE_H

#include <vector>

#include "State.h"
#include "util_SDL.h"

class MultiState
    : public State
{
    public:
        MultiState(const State* a_parent)
            : State(a_parent)
        {
        }

        void add(State_ptr a_child)
        {
            a_child->activate();
            m_children.push_back(a_child);
        }

        virtual void update() override
        {
            bool nulls = false;

            do
            {
                auto p = std::find(m_children.begin(), m_children.end(), nullptr);
                nulls = p != m_children.end();
                if (nulls) m_children.erase(p);
            } while (nulls);

            if ( m_children.size() == 0 )
                return;

            for ( unsigned int i = 0; i < m_children.size(); i++ )
            {
                auto s = m_children[i];

                if ( s == nullptr ) continue;

                if ( s->getStatus() == AppState::running )
                    s->update();
                else if ( s->getStatus() == AppState::finished )
                {
                    s->cleanup();
                    s = s->getNextState();

                    if ( s != nullptr )
                        s->activate();
                }
            }
        }

        virtual void draw(Surface_ptr a_parent) override
        {
            for ( auto i = m_children.begin(); i != m_children.end(); i++ )
            {
                m_child = *i;
                State::draw(a_parent);
            }
            m_child = nullptr;
        }

    private:
        std::vector<State_ptr> m_children;
};

#endif 
