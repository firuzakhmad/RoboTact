#include "core/utils/logger/logger.hpp"
#include <core/utils/timer/timer.hpp>
#include "core/utils/service_locator/service_locator.hpp"

#include <stdexcept>
#include <memory>

using namespace RoboTact;

[[noreturn]] int main()
{
	auto timer = std::make_shared<Core::Timer>();

	auto logger = std::make_shared<Core::Logger>();
	logger->init("application.log", Core::LogLevel::TRACE);

	Core::ServiceLocator::register_service<Core::ILogger>(logger);
	Core::ServiceLocator::register_service<Core::ITimer>(timer);
	
	try
	{
		timer->reset();
		double test = 1.0; // one second delay test

		while (true) 
		{
        	timer->update();
        
	        // Fixed timestep physics
	        while (timer->get_accumulated_time() >= test) 
	        {
				LOG_TRACE("LOG_TRACE Test");   
				LOG_DEBUG("LOG_DEBUG Test");   
				LOG_INFO("LOG_INFO Test");    
				LOG_WARNING("LOG_WARNING Test"); 
				LOG_ERROR("LOG_ERROR Test");   
				LOG_FATAL("LOG_FATAL Test");

	            timer->consume_accumulated_time(test);
	        }
        }

	}
	catch(const std::exception& e)
	{
		LOG_FATAL("Unhandled exception: ", e.what());
	}
}