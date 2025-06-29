#include "timer.hpp"

namespace RoboTact::Core
{

Timer::Timer()
	: m_start_time{Clock::now()},
	  m_last_time{m_start_time},
	  m_delta_time{0.0},
	  m_elapsed_time{0.0},
	  m_accumulated_time{0.0}
{
	// Ensuring proper initialization of atomic variables
	m_delta_time.store(0.0);	
	m_elapsed_time.store(0.0);	
	m_accumulated_time.store(0.0);	
}

void Timer::reset() 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_start_time = m_last_time = Clock::now();
    m_delta_time.store(0.0);
    m_elapsed_time.store(0.0);
    m_accumulated_time.store(0.0);
}

void Timer::update() {
  std::lock_guard<std::mutex> lock(m_mutex);

  const auto now = Clock::now();
  const Duration delta = now - m_last_time;

  // Update timing values using relaxed memory ordering.
  m_delta_time.store(delta.count(), std::memory_order_relaxed);
  m_elapsed_time.store(
  		m_elapsed_time.load(std::memory_order_relaxed) + delta.count(),
		std::memory_order_relaxed);
  m_accumulated_time.store(
		m_accumulated_time.load(std::memory_order_relaxed) + delta.count(),
		std::memory_order_relaxed);

  m_last_time = now;
}

void Timer::consume_accumulated_time(double time)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_accumulated_time.store(m_accumulated_time.load(std::memory_order_relaxed) - time,
		std::memory_order_relaxed);
}

double Timer::get_delta_time() const 
{
	return m_delta_time.load(std::memory_order_relaxed);
}

double Timer::get_elapsed_time() const 
{
	return m_elapsed_time.load(std::memory_order_relaxed);
}

double Timer::get_accumulated_time() const 
{
	return m_accumulated_time.load(std::memory_order_relaxed);
}

} // namespace RoboTact::Core