#ifndef FIVE_STATE_IO_H
#define FIVE_STATE_IO_H

#include<vector>
using namespace std;

#include "process.h"

// gives each interrupt the info on what its own ID is and what process ID its associated with
struct IOInterrupt
{
    IOInterrupt() : ioEventID(99999), procID(99999) {};
    IOInterrupt(const unsigned int& eId, const unsigned int& pId) : ioEventID(eId), procID(pId) {};

    unsigned int ioEventID;
    unsigned int procID;
};

// The IOModule object keeps track of the IOInterrupts in a doubly linked list
class IOModule
{
    public:
      // TO CLARIFY: m_intVec is tied to the doubly linked IOInterrupt list passed to the IOModule object at instantiation
      IOModule(list<IOInterrupt>& ioIntVec) : m_intVec(ioIntVec) {}

      inline void ioProcessing(const int& curTimeStep)
      {
        // Loops for however many IOInterrupts are in IOModule.m_pending
        for(int i = 0, i_end = m_pending.size(); i < i_end; ++i)
        {
            if(curTimeStep == m_pending[i].first)
            {
                // Add IOInterrupt to back of IOModule.m_pending
                m_intVec.push_back(m_pending[i].second);

                // Removes the i'th element from m_pending (the member variable)
                m_pending.erase(m_pending.begin() + i);


                // Say m_pending size was 3 when the for loop began --> loop ends when i == 3
                // That means the new m_pending size is actually 2 (because of m_pending.erase())

                // because we're modifying m_pending by removing the <int, IOInterrupt> at position i,
                // we need to remove an iteration from the for loop (so we don't walk off the vector)
                --i;
                --i_end;
            }
        }
      }

      inline void submitIORequest(const int& curTimeStep, const IOEvent& ioEvent, const Process& proc)
      {
        m_pending.push_back(make_pair(curTimeStep + ioEvent.duration, IOInterrupt(ioEvent.id, proc.id)));
      }

    private:
      // A reference to the IOInterrupt doubly linked list, as a member variable
      list<IOInterrupt>& m_intVec;

      // a dynamically allocated array wherein each element is a pair of two other types, an int and an IOInterrupt
      // FOR IOINTERRUPTS!!! NOT PROCESSES
      vector<pair<int, IOInterrupt> > m_pending; 
};

#endif