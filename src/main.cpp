#include "core/utils/logger/logger.hpp"

#include <stdexcept>
#include <memory>

using namespace RoboTact;

[[noreturn]] int main()
{
	auto logger = std::make_unique<Core::Logger>();
	
	try
	{
		logger->init("application.log", Core::LogLevel::TRACE);

		LOG_TRACE("LOG_TRACE Test");   
		LOG_DEBUG("LOG_DEBUG Test");   
		LOG_INFO("LOG_INFO Test");    
		LOG_WARNING("LOG_WARNING Test"); 
		LOG_ERROR("LOG_ERROR Test");   
		LOG_FATAL("LOG_FATAL Test");
	}
	catch(const std::exception& e)
	{
		LOG_FATAL("Unhandled exception: ", e.what());
	}

}