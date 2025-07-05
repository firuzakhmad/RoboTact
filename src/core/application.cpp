#include "application.hpp"
#include "core/utils/logger/logger.hpp"
#include "core/utils/timer/timer.hpp"
#include "core/utils/service_locator/service_locator.hpp"

namespace RoboTact
{
Application::Application()
{
    initialize_services();

    LOG_INFO("Application initializing...");
    
    m_window = std::make_unique<Core::SDLWindow>();
}

Application::~Application()
{
	request_stop();
	LOG_INFO("Application destroyed.");
}	

bool Application::should_continue() const noexcept
{
    auto thread_manager = Core::ServiceLocator::resolve<Core::ThreadManager>();

    return thread_manager->should_continue() && !m_window->should_close();
}

void Application::initialize_services()
{
    auto timer = std::make_shared<Core::Timer>();
    timer->reset();

    auto logger = std::make_shared<Core::Logger>();
    logger->init("application.log", Core::LogLevel::TRACE);

    auto thread_manager = std::make_shared<Core::ThreadManager>();

    Core::ServiceLocator::register_service<Core::ITimer>(timer);
    Core::ServiceLocator::register_service<Core::ILogger>(logger);
    Core::ServiceLocator::register_service<Core::ThreadManager>(thread_manager);
}


void Application::run()
{
	LOG_INFO("Starting main, simulation, and IO threads.");
    
    auto thread_manager = Core::ServiceLocator::resolve<Core::ThreadManager>();

	// Start threads through the thread manager
    thread_manager->start_thread(
        Core::ThreadManager::ThreadType::SIMULATION,
        std::bind(&Application::simulation_loop, this)
    );
    
    thread_manager->start_thread(
        Core::ThreadManager::ThreadType::IO,
        std::bind(&Application::io_loop, this)
    );

    // Run main loop in current thread
    main_loop();

	LOG_INFO("All threads joined, application exiting.");
}

void Application::request_stop()
{
    auto thread_manager = Core::ServiceLocator::resolve<Core::ThreadManager>();

    thread_manager->stop_all();
}

void Application::main_loop()
{
    LOG_INFO("Main thread started.");

	auto timer = Core::ServiceLocator::resolve<Core::ITimer>();

    while (should_continue())
    {
		timer->update();
		double delta_time = timer->get_delta_time();

		m_window->poll_events();

        double fixed_timestep = 1.0 / 60.0;
        while (timer->get_accumulated_time() >= fixed_timestep) 
        {
            
            timer->consume_accumulated_time(fixed_timestep);
        }

        m_window->swap_buffers();
    }
    LOG_INFO("Main thread exiting.");
}

void Application::simulation_loop()
{
    LOG_INFO("Simulation thread started.");
    while (should_continue())
    {

        // Approx 60 Hz simulation update rate
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }   
    LOG_INFO("Simulation thread exiting.");
}

void Application::io_loop()
{
    LOG_INFO("IO thread started.");
    while (should_continue())
    {
        // Higher frequency IO polling (100 Hz)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    LOG_INFO("IO thread exiting.");
}

} // namespace RoboTact