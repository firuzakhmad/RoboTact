#ifndef LOGGER_HPP
#define LOGGER_HPP

/**
 * @brief Logging system with file and console output
 * 
 * Features:
 * - Thread-safe logging
 * - Multiple log levels (TRACE to FATAL)
 * - Color-coded console output
 * - Timestamp precision to milliseconds
 * - Interface-based design for testability
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <memory>
#include <atomic>
#include <format>

namespace RoboTact::Core
{

/**
 * @enum LogLevel
 * @brief Severity levels for log messages
 * 
 * Messages with lower severity than the current threshold will be filtered out.
 */
enum class LogLevel
{
	TRACE,   
    DEBUG,   
    INFO,    
    WARNING, 
    ERROR,   
    FATAL    
};

/**
 * @class ILogger
 * @brief Abstract interface for logging functionality
 * 
 * Provides a contract for logger implementations, enabling dependency injection
 * and easy mocking for unit testing.
 */
class ILogger
{
public: 
	virtual ~ILogger() = default;

	/**
	 * @brief Initialize the logger
	 * @param file_name Path to log file (emtpy for no file logging)
	 * @param level Minimum severity level to log
	 */
	virtual void init(const std::string& file_name, LogLevel level = LogLevel::INFO) = 0;

	/**
	 * @brief Set the minimum log level
	 * @param level Message below this level will be filtered
	 */
	virtual void set_log_level(LogLevel level) = 0; 

    /**
     * @brief Log a message with specified severity level
     * @param level Severity level of the message
     * @param message The message to log
     */
    virtual void log(LogLevel level, const std::string& message) = 0;

private:
	/**
     * @brief Converts a single argument to its string representation.
     * 
     * Uses stringstream to convert any streamable type into a std::string.
     *
     * @tparam T Type of the argument.
     * @param arg The argument to convert.
     * @return String representation of the argument.
     */
    template<typename T>
    static std::string Format(T&& arg) 
    {
        std::ostringstream oss;
        oss << std::forward<T>(arg);
        return oss.str();
    }

    /**
     * @brief Recursively formats multiple arguments into a space-separated string.
     * 
     * This function combines all arguments into a single string by converting each
     * argument to a string and separating them with spaces.
     * 
     * @tparam T Type of the first argument.
     * @tparam Args Types of the remaining arguments.
     * @param arg The first argument.
     * @param args Remaining arguments.
     * @return A space-separated concatenated string representation of all arguments.
     */
    template<typename T, typename... Args>
    static std::string Format(T&& arg, Args&&... args) 
    {
        return Format(std::forward<T>(arg)) + " " + Format(std::forward<Args>(args)...);
    }
};


/**
 * @class Logger
 * @brief Concrete thread-safe logger implementation
 * 
 * Supports simultaneous console (with color) and file output with configurable
 * log level filtering.
 */
class Logger : public ILogger 
{
public:
    /**
     * @brief Construct a new Logger object
     * 
     * Note: Logger must be initialized with init() before use
     */
    Logger() = default;

    /**
     * @brief Destroy the Logger object
     * 
     * Ensures log file is properly closed
     */
    ~Logger() override;

    // Delete copy/move operations to ensure thread safety
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * @copydoc ILogger::init
     */
    void init(const std::string& file_name, LogLevel level = LogLevel::INFO) override;

    /**
     * @copydoc ILogger::set_log_level
     */
    void set_log_level(LogLevel level) override;

    /**
     * @copydoc ILogger::log
     */
    void log(LogLevel level, const std::string& message) override;

    /**
     * @brief Logs a message composed from multiple arguments at the specified severity level.
     * 
     * This is a convenience templated overload that formats the arguments into a single string,
     * prefixes it with a timestamp and log level, and then logs the result to console and file.
     * 
     * Note: This method is not virtual and is only available through the Logger class directly.
     *       If logging via an ILogger* pointer, use the string-based log() method instead.
     * 
     * @tparam Args Types of the message arguments
     * @param level Severity level of the log message
     * @param args Arguments to format and log
     */
    template<typename... Args>
    void log(LogLevel level, Args&&... args)
    {
        // Early exit if message level is below threshold
        if (level < m_log_level.load()) { return; }

        // Format the log message
        std::ostringstream oss;
        oss << get_time_stamp() 
            << " [" << log_level_to_string(level) << "] " 
            << Format(std::forward<Args>(args)...);;


        // Thread-safe output operations
        std::lock_guard<std::mutex> lock(m_mutex);
        
        const std::string log_message = oss.str();
        
        // Colorized console output
        std::cout << log_level_to_color(level) 
                  << log_message 
                  << "\033[0m" << std::endl; // Set back console color to default

        // File output (if enabled)
        if (m_log_file.is_open()) 
        {
            m_log_file << log_message << std::endl;
        }
    }

private:
    static std::string get_time_stamp();
    static std::string log_level_to_string(LogLevel level);
    static std::string log_level_to_color(LogLevel level);

    std::ofstream m_log_file;          
    std::atomic<LogLevel> m_log_level; 
    std::mutex m_mutex;                
};


/**
 * @class NullLogger
 * @brief Null object pattern implementation of ILogger
 * 
 * Provides a no-op logger for testing or when logging needs to be disabled.
 */
class NullLogger : public ILogger 
{
public:
    void init(const std::string&, LogLevel) override {}
    void set_log_level(LogLevel) override {}
    void log(LogLevel, const std::string&) override {}
};

// Convenience logging macros
#define LOG_TRACE(...)   logger->log(RoboTact::Core::LogLevel::TRACE, __VA_ARGS__)
#define LOG_DEBUG(...)   logger->log(RoboTact::Core::LogLevel::DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    logger->log(RoboTact::Core::LogLevel::INFO, __VA_ARGS__)
#define LOG_WARNING(...) logger->log(RoboTact::Core::LogLevel::WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   logger->log(RoboTact::Core::LogLevel::ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   logger->log(RoboTact::Core::LogLevel::FATAL, __VA_ARGS__)

} // namespace RoboTact::Core

#endif // LOGGER_HPP