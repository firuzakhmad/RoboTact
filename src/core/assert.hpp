#ifndef ROBOT_ASSERT_HPP
#define ROBOT_ASSERT_HPP

#include <iostream>
#include <cstdlib>
#include "logger.hpp"  

// Platform-specific debug break
#ifdef _MSC_VER
    #define DEBUG_BREAK() __debugbreak()
#elif defined(__GNUC__) || defined(__clang__)
    #define DEBUG_BREAK() __builtin_trap()
#else
    #define DEBUG_BREAK() std::abort()
#endif

namespace RoboTact
{
    inline void log_assertion_failure(const char* expr, const char* file, int line, const char* function)
    {
        std::cerr << "[ASSERTION FAILED] " << expr << "\n"
                  << "Location: " << file << ":" << line << "\n"
                  << "Function: " << function << "\n";
    }

    // General-purpose assertion macro
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

    // Optional macro for checking sensor or hardware errors
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

} // namespace RoboTact

#endif // ROBOT_ASSERT_HPP
