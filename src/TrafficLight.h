#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase {red, green};

// The class „MessageQueue“ has the public methods send and receive. 
// Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// Also, there is an std::condition_variable as well as an std::mutex as private members. 

template <class T>
class MessageQueue
{
public:
    T receive();
    void send (T &&msg);

private:
    std::mutex _mutex;
    std::condition_variable _cond;
    std::deque<T> _queue;
    
};

// The class „TrafficLight“ which is a child class of TrafficObject. 
// The class has the public methods „void waitForGreen()“ and „void simulate()“ 
// as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// Furthermore, there is a private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    //~TrafficLight(){};
    // getters / setters
    TrafficLightPhase getCurrentPhase();  //gettter

    //void set_phase(TrafficLightPhase phase);     //setter

    // typical behaviour methods
    void waitForGreen();
    void simulate() override;
    

private:
    // typical behaviour methods

    // Private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    void cycleThroughPhases();

    //std::shared_ptr<MessageQueue<TrafficLightPhase>> queue;

    MessageQueue<TrafficLightPhase> m_queue;

    TrafficLightPhase _currentPhase;

    std::condition_variable _condition;
    std::mutex _mutex;
};



#endif
