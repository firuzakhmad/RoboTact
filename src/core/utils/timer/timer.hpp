#ifndef TIMER_HPP
#define TIMER_HPP

/**
 * @brief High-resolution timing system for application and game loops
 * and performance measurement
 *
 * Features:
 * - Nanosecond precision timing
 * - Thread-safe operations
 * - Support for fixed timestep accumulation
 * - Interface-base design for testability
 * - Platform-independent high resolution clock
 */

#include <chrono>
#include <atomic>
#include <mutex>
#include <memory>

namespace RoboTact::Core
{

/**
 * @class ITimer
 * @brief Abstract interface for timing functionality
 *
 * Provides a contract for timing implementation enabling dependency injection
 */
class ITimer
{
public:
	virtual ~ITimer() = default;

	/**
     * @brief Reset all timer values to zero
     */
    virtual void reset() = 0;

    /**
     * @brief Update timer with current frame time
     * 
     * Should be called once per frame to update delta time calculations
     */
    virtual void update() = 0;

    /**
     * @brief Consume accumulated time for fixed timestep simulations
     * @param time Amount of time to consume from the accumulator
     */
    virtual void consume_accumulated_time(double time) = 0;

    /**
     * @brief Get time since last update (in seconds)
     * @return Delta time in seconds with nanosecond precision
     */
    virtual double get_delta_time() const = 0;

    /**
     * @brief Get total elapsed time since last reset (in seconds)
     * @return Elapsed time in seconds with nanosecond precision
     */
    virtual double get_elapsed_time() const = 0;

    /**
     * @brief Get accumulated time for fixed timestep simulations
     * @return Accumulated time in seconds
     */
    virtual double get_accumulated_time() const = 0;
};

/**
 * @class Timer
 * @brief Concrete high-resolution timer implementation
 * 
 * Provides precise timing measurements suitable for application and game loops,
 * physics simulations, and performance profiling.
 */
class Timer : public ITimer
{
public:
	/**
     * @brief Constructs a new Timer object
     * 
     * Initializes all timing values to zero and captures the start time
     */
    Timer();

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

    /**
     * @copydoc ITimer::reset
     */
    void reset() override;

    /**
     * @copydoc ITimer::update
     */
    void update() override;

    /**
     * @copydoc ITimer::consume_accumulated_time
     */
    void consume_accumulated_time(double time) override;

    /**
     * @copydoc ITimer::get_delta_time
     */
    double get_delta_time() const override;

    /**
     * @copydoc ITimer::get_elapsed_time
     */
    double get_elapsed_time() const override;

    /**
     * @copydoc ITimer::get_accumulated_time
     */
    double get_accumulated_time() const override;
	
private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    TimePoint m_start_time;        
    TimePoint m_last_time;         
    std::atomic<double> m_delta_time;      
    std::atomic<double> m_elapsed_time;    
    std::atomic<double> m_accumulated_time;
    mutable std::mutex m_mutex;   
};

/**
 * @class MockTimer
 * @brief Test implementation of ITimer
 * 
 * Provides predictable timing values for unit testing
 */
class MockTimer : public ITimer {
public:
    void reset() override {}
    void update() override {}
    void consume_accumulated_time(double) override {}
    double get_delta_time() const override { return 0.016667; } // 60 FPS
    double get_elapsed_time() const override { return 0.0; }
    double get_accumulated_time() const override { return 0.0; }
};

} // namespace RoboTact::Core

#endif // TIMER_HPP