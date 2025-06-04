#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <memory>
#include <mutex>
#include <atomic>

namespace RoboTact
{
	class Timer
	{
	public:
		
		Timer(const Timer&) = delete;
		Timer& operator=(const Timer&) = delete;

		static Timer& get_instance()
		{
			static Timer instance;
			return instance;
		}

	    void reset();
	    void update();
	    void consume_accumulated_time(double time);

	    [[nodiscard]] double get_delta_time() const {
	        return m_delta_time.load();
	    }

	    [[nodiscard]] double get_elapsed_time() const {
	        return m_elapsed_time.load();
	    }

	    [[nodiscard]] double get_accumulated_time() const {
	        return m_accumulated_time.load();
	    }
	private:
		using Clock = std::chrono::high_resolution_clock;

		// Timing data
		std::chrono::time_point<Clock> m_start_time;      // Time when the timer was last reset
		std::chrono::time_point<Clock> m_last_time;       // Time of the last update
		std::atomic<double> m_delta_time;                 // Time since the last update
		std::atomic<double> m_elapsed_time;               // Total elapsed time since reset
		std::atomic<double> m_accumulated_time;           // Accumulated time for fixed timestep simulations

		// Mutex for thread safety
		mutable std::mutex m_mutex;

		Timer();
	};

} // namespace RoboTact

#endif
