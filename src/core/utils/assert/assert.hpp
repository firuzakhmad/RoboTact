/**
 * @brief RoboTact's runtime diagnostic and failure handling system
 * 
 * Provides hardened assertion macros for:
 * - Debugging (RA_ASSERT)
 * - Hardware/sensor validation (RA_CHECK_SUCCESS)
 * - Platform-aware debug traps
 * 
 * Key Features:
 * - Detailed failure context (file, line, function)
 * - Integrated with RoboTact logging system
 * - Immediate termination on critical failures
 * - Cross-platform debug break support
 * 
 * Usage:
 * @code
 * RA_ASSERT(ptr != nullptr, "Received null pointer");
 * RA_CHECK_SUCCESS(sensor_init(), "Sensor initialization failed");
 * @endcode
 */

#ifndef ROBOT_ASSERT_HPP
#define ROBOT_ASSERT_HPP

#include "core/utils/logger/logger.hpp"  
#include "core/utils/service_locator/service_locator.hpp"  

#include <iostream>
#include <cstdlib>

/**
 * @def DEBUG_BREAK()
 * @brief Platform-specific debug trap instruction
 * 
 * Behavior:
 * - Windows: __debugbreak() (MSVC)
 * - Unix: __builtin_trap() (GCC/Clang)
 * - Fallback: std::abort()
 */
#ifdef _MSC_VER
    #define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #define DEBUG_BREAK() __builtin_trap()
#else
    #define DEBUG_BREAK() std::abort()
#endif

namespace RoboTact::Core
{

/**
 * @brief Logs assertion failure details to stderr
 * @param expr The failed expression as string
 * @param file Source file name
 * @param line Source line number
 * @param function Enclosing function name
 * 
 * @note Used internally by RA_ASSERT macro
 */
inline void log_assertion_failure(const char* expr, const char* file, int line, const char* function)
{
    std::cerr << "[ASSERTION FAILED] " << expr << "\n"
              << "Location: " << file << ":" << line << "\n"
              << "Function: " << function << "\n";
}

/**
 * @def RA_ASSERT(expr, ...)
 * @brief Runtime assertion with optional message
 * 
 * Behavior:
 * 1. Evaluates expression
 * 2. On failure:
 *    - Logs to both stderr and RoboTact logging system
 *    - Triggers debug break
 *    - Terminates program
 * 
 * @param expr Boolean expression to test
 * @param ... Optional format string and arguments
 */
#define RA_ASSERT(expr, ...) \
do { \
    if (!(expr)) { \
        LOG_FATAL("Assertion failed: ", #expr, \
                  "\n  Message: ", ##__VA_ARGS__, \
                  "\n  Location: ", __FILE__, ":", __LINE__, \
                  "\n  Function: ", __FUNCTION__); \
        DEBUG_BREAK(); \
        std::abort(); \
    } \
} while (false)

/**
 * @def RA_CHECK_SUCCESS(call, ...)
 * @brief Validates hardware/sensor operation results
 * 
 * Specialized for:
 * - Device driver calls
 * - Hardware interface verification
 * - Critical system operations
 * 
 * @param call Function call returning error code (0=success)
 * @param ... Optional context message
 */
#define RA_CHECK_SUCCESS(call, ...) \
do { \
    auto result = call; \
    if (result) { \
        LOG_FATAL("[RoboTact Error] ", #call, " returned failure", \
                  " at ", __FILE__, ":", __LINE__ \
                  __VA_OPT__(, "\nContext: ", __VA_ARGS__)); \
        DEBUG_BREAK(); \
        std::abort(); \
    } \
} while (false)

} // namespace RoboTact::Core

#endif // ROBOT_ASSERT_HPP
