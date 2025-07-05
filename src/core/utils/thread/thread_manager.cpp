#include "thread_manager.hpp"

namespace RoboTact::Core
{
	ThreadManager::ThreadManager() 
		: m_running(true) 
	{
		unsigned num_workers = std::thread::hardware_concurrency();
		if (num_workers == 0) num_workers = 2;

		for (unsigned i = 0; i < num_workers; ++i)
		{
			m_threads.emplace_back(
		        ThreadType::MAIN,
		        std::thread(&ThreadManager::worker_loop, this),
		        true
		    );
		}
	}

	ThreadManager::~ThreadManager() { stop_all(); }

	void ThreadManager::stop_all() 
	{
	    if (!m_running) return;
	    
	    LOG_INFO("Stopping all threads gracefully...");
	    
	    // Signal task workers to stop
	    {
	        std::unique_lock<std::mutex> lock(m_task_mutex);
	        m_stop_tasks = true;
	    }
	    m_task_cv.notify_all();
	    
	    // Signal other threads
	    m_running = false;
	    
	    // Join all threads
	    for (auto& thread_info : m_threads) 
	    {
	        if (thread_info.thread.joinable()) 
	        {
	            thread_info.running = false;
	            thread_info.thread.join();
	        }
	    }
	    
	    m_threads.clear();
	}

	void ThreadManager::emergency_stop() 
	{
	    LOG_ERROR("EMERGENCY STOP INITIATED!");
	    m_emergency_stop = true;
	    stop_all();
	}

	bool ThreadManager::should_continue() const noexcept
	{
    	return m_running && !m_emergency_stop;
	}

	void ThreadManager::start_thread(ThreadType type, std::function<void()> func) 
	{
	    m_threads.emplace_back(
	        type,
		    std::thread([this, func, type]() 
		    {
		        while (should_continue())
		        {
		            try {
		                func();
		            } catch (const std::exception& e) {
		                LOG_ERROR("Exception in thread: {}", e.what());
		                if (type == ThreadType::MAIN) {
		                    emergency_stop();
		                    break;
		                }
		            }
		        }
		    }),
	        true
	    );
	}

	void ThreadManager::worker_loop() 
	{
	    while (should_continue()) 
	    {
	        std::function<void()> task;
	        
	        {
	            std::unique_lock<std::mutex> lock(m_task_mutex);
	            m_task_cv.wait(lock, [this]{
	                return !m_tasks.empty() || m_stop_tasks;
	            });
	            
	            if (m_stop_tasks && m_tasks.empty()) 
	            {
	                return;
	            }
	            
	            task = std::move(m_tasks.front());
	            m_tasks.pop();
	        }
	        
	        try {
	            task();
	        } catch (const std::exception& e) {
	            LOG_ERROR("Exception in task: {}", e.what());
	        }
	    }
	}



} // namespace RoboTact::Core