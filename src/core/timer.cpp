#include "timer.hpp"

namespace RoboTact {

    Timer::Timer() 
    	: m_start_time(Clock::now()), 
    	  m_last_time(m_start_time), 
    	  m_delta_time(0.0), 
    	  m_elapsed_time(0.0), 
    	  m_accumulated_time(0.0) {}

    void Timer::reset() 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_start_time = m_last_time = Clock::now();
        m_delta_time = 0.0;
        m_elapsed_time = 0.0;
        m_accumulated_time = 0.0;
    }

    void Timer::update() 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto now = Clock::now();
        double delta = std::chrono::duration<double>(now - m_last_time).count();

        m_delta_time.store(delta);
        m_elapsed_time.store(m_elapsed_time.load() + delta);
        m_accumulated_time.store(m_accumulated_time.load() + delta);
        m_last_time = now;
    }

    void Timer::consume_accumulated_time(double time) 
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_accumulated_time.store(m_accumulated_time.load() - time);
    }

} // namespace RoboTact
