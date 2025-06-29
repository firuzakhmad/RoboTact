#include "core/utils/logger/logger.hpp"
#include <core/utils/timer/timer.hpp>

#include <stdexcept>
#include <memory>

using namespace RoboTact;

[[noreturn]] int main()
{

	auto logger = std::make_unique<Core::Logger>();
	auto timer = std::make_unique<Core::Timer>();
	
	try
	{
		timer->reset();
		double test = 1.0;

		logger->init("application.log", Core::LogLevel::TRACE);

		while (true) 
		{
        	timer->update();
        
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