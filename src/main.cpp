#include "core/application.hpp"
#include "core/utils/logger/logger.hpp"


using namespace RoboTact;

[[noreturn]] int main()
{
	Application app{};

	try
	{
		app.run();
	} 
	catch(const std::exception& e) 
	{
		LOG_FATAL("Unhandled exception: ", e.what());
	}
}