#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // The method receive should uses std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object is returned by the receive function. 

    // perform queue modification under the lock
    std::unique_lock<std::mutex> uLock(_mutex);
    //_cond.wait(uLock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable
    
    _cond.wait(uLock);
    // remove last vector element from queue
    T msg = std::move(_queue.back());
    _queue.pop_back();

    return msg; // will not be copied due to return value optimization (RVO) in C++
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // The method send uses the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

        //delay to expose data races
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // perform vector modification under the lock
        std::lock_guard<std::mutex> uLock(_mutex);
        _queue.clear();
        // add vector to queue
        //std::cout << "   Message " << msg << " has been sent to the queue" << std::endl;
        _queue.push_back(std::move(msg));
        _cond.notify_one(); // notify client after pushing new Vehicle into vector
}


/* Implementation of class "TrafficLight" */

 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    
}

void TrafficLight::waitForGreen()
{
    // The method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    TrafficLightPhase curr;
    while (true)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
        curr = m_queue.receive();

        if(curr == TrafficLightPhase::green)
            return;

    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // the private method „cycleThroughPhases“ is started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    TrafficObject::threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration is a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

        std::random_device dev;
        std::mt19937 gen(dev());
        std::uniform_int_distribution<> distr(4000, 6000);

        int cycleDuration = distr(gen);

        std::chrono::high_resolution_clock::time_point lastUpdate = std::chrono::high_resolution_clock::now();

    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();

        auto timeSinceLastUpdate= std::chrono::duration_cast<std::chrono::milliseconds>(Now - lastUpdate).count();



        if (timeSinceLastUpdate >= cycleDuration)
        {

            //color toggle
            if (_currentPhase == TrafficLightPhase::red)
            {
                _currentPhase = TrafficLightPhase::green;
            }
            else
            {
                _currentPhase = TrafficLightPhase::red;
            }

            //send update message to queue
            m_queue.send(std::move(_currentPhase));
            // reset stop watch for next cycle

            lastUpdate = std::chrono::high_resolution_clock::now();
          	Now = std::chrono::high_resolution_clock::now();

            //re-rand
          	cycleDuration = distr(gen);

        }

    }



    
}

