#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <memory>

#ifndef LOGGER_HPP
#define LOGGER_HPP

namespace RoboTact 
{
	enum class LogLevel
	{
		TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
	};

	class Logger
	{
	public:
		static Logger& get_instance() 
		{
			static Logger m_instance;
			return m_instance;
		}

		void init(const std::string& file_name, LogLevel level = LogLevel::INFO) 
		{
	        std::lock_guard<std::mutex> lock(m_mutex);
	        m_log_file.open(file_name, std::ios::out | std::ios::app);
	        m_log_level = level;
	    }

	    void set_log_level(LogLevel level) 
	    {
	        std::lock_guard<std::mutex> lock(m_mutex);
	        m_log_level = level;
	    }

	    template<typename... Args>
	    void log(LogLevel level, Args&&... args) 
	    {
	        if (level < m_log_level) return;

	        std::ostringstream oss;
	        oss << get_time_stamp() << " [" << log_level_to_string(level) << "] " << Format(std::forward<Args>(args)...);

	        std::lock_guard<std::mutex> lock(m_mutex);
	        
	        std::string log_message = oss.str();

	        // Print to console with color
	        std::cout << log_level_to_color(level) << log_message << "\033[0m" << std::endl;

	        // Write to file
	        if (m_log_file.is_open()) {
	            m_log_file << log_message << std::endl;
	        }
	    }

		~Logger() 
		{
	        if (m_log_file.is_open()) m_log_file.close();
	    }
		
	private:
		std::ofstream 	m_log_file;
		LogLevel 		m_log_level = LogLevel::INFO;
		std::mutex 		m_mutex;

		Logger() = default;
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		static std::string get_time_stamp()
		{
			using namespace std::chrono;
			auto now = system_clock::now();
			auto t_time = system_clock::to_time_t(now);
			auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

			std::ostringstream oss;
			oss << std::put_time(std::localtime(&t_time), "%Y-%m-%d %H:%M:%S")
				<< '.' << std::setw(3) << std::setfill('0') << ms.count();
			return oss.str();
		}

		static std::string log_level_to_string(LogLevel level)
		{
			switch (level) 
			{
	            case LogLevel::TRACE:   return "TRACE";
	            case LogLevel::DEBUG:   return "DEBUG";
	            case LogLevel::INFO:    return "INFO";
	            case LogLevel::WARNING: return "WARNING";
	            case LogLevel::ERROR:   return "ERROR";
	            case LogLevel::FATAL:   return "FATAL";
        	}
        	return "UNKNOWN";
		}


	    static std::string log_level_to_color(LogLevel level) 
	    {
	        switch (level) 
	        {
	            case LogLevel::TRACE:   return "\033[37m";  // White
	            case LogLevel::DEBUG:   return "\033[35m";  // Cyan
	            case LogLevel::INFO:    return "\033[32m";  // Green
	            case LogLevel::WARNING: return "\033[33m";  // Yellow
	            case LogLevel::ERROR:   return "\033[31m";  // Red
	            case LogLevel::FATAL:   return "\033[41m\033[97m"; // Red Background, White Text
	        }
	        return "\033[0m"; // Reset
	    }

	    template<typename T>
	    static std::string Format(T&& arg) 
	    {
	        std::ostringstream oss;
	        oss << std::forward<T>(arg);
	        return oss.str();
	    }

	    template<typename T, typename... Args>
	    static std::string Format(T&& arg, Args&&... args) 
	    {
	        return Format(std::forward<T>(arg)) + " " + Format(std::forward<Args>(args)...);
	    }
	};

	// Logging Macros
	#define LOG_TRACE(...)   Logger::get_instance().log(LogLevel::TRACE, __VA_ARGS__)
	#define LOG_DEBUG(...)   Logger::get_instance().log(LogLevel::DEBUG, __VA_ARGS__)
	#define LOG_INFO(...)    Logger::get_instance().log(LogLevel::INFO, __VA_ARGS__)
	#define LOG_WARNING(...) Logger::get_instance().log(LogLevel::WARNING, __VA_ARGS__)
	#define LOG_ERROR(...)   Logger::get_instance().log(LogLevel::ERROR, __VA_ARGS__)
	#define LOG_FATAL(...)   Logger::get_instance().log(LogLevel::FATAL, __VA_ARGS__)

} // namespace RoboTact

#endif // LOGGER_HPP