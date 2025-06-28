#include "logger.hpp"

namespace RoboTact::Core
{

Logger::~Logger()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_log_file.is_open())
    {
        m_log_file.close();
    }
}

void Logger::init(const std::string& file_name, LogLevel level)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Close the existing log file if open
    if (m_log_file.is_open())
    {
        m_log_file.close();
    }

    // Open new log file if specified filename
    if (!file_name.empty())
    {
        m_log_file.open(file_name, std::ios::out | std::ios::app);
        if (!m_log_file.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + file_name);
        }
    }

    m_log_level.store(level);
}

void Logger::set_log_level(LogLevel level)
{
    m_log_level.store(level);
}

void Logger::log(LogLevel level, const std::string& message) 
{
    // Early exit if message level is below threshold
    if (level < m_log_level.load()) { return; }

    // Format the log message
    std::ostringstream oss;
    oss << get_time_stamp() 
        << " [" << log_level_to_string(level) << "] " 
        << message;


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

std::string Logger::get_time_stamp() 
{
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto t_time = system_clock::to_time_t(now);
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t_time), "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

std::string Logger::log_level_to_string(LogLevel level) 
{
    switch (level) 
    {
        case LogLevel::TRACE:   return "TRACE";
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARN";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

std::string Logger::log_level_to_color(LogLevel level) 
{
    switch (level) 
    {
        case LogLevel::TRACE:   return "\033[37m";       // White
        case LogLevel::DEBUG:   return "\033[36m";       // Cyan
        case LogLevel::INFO:    return "\033[32m";       // Green
        case LogLevel::WARNING: return "\033[33m";       // Yellow
        case LogLevel::ERROR:   return "\033[31m";       // Red
        case LogLevel::FATAL:   return "\033[41m\033[97m"; // Red bg, White text
        default:                return "\033[0m";        // Reset
    }
}

} //namespace RoboTact::Core