#ifndef SERVICE_LOCATOR_HPP
#define SERVICE_LOCATOR_HPP

#include <memory>
#include <unordered_map>
#include <mutex>
#include <functional>
#include <typeindex>
#include <stdexcept>

namespace RoboTact::Core
{

/**
 * @class ServiceLocator
 * @brief Thread-safe, static dependency injection container for
 * for managing and resolving service instance.
 *
 * Implements the Service Locator pattern to decouple service 
 * consumers from concrete implementation.
 *
 * Supports:
 * - Instance registration (`register_service`).
 * - Lazy factory registration (`register_service_factory`).
 * - Thread-safe resolution (`resolve`).
 * - Dynamic service removal (`unregister_service`).
 *
 * @warning All operations are static and the class is 
 * non-copyable and non-movable.
 */
class ServiceLocator
{
public:

	// Copy and move instance prohibition.
	ServiceLocator() = delete;
	ServiceLocator(const ServiceLocator&) = delete;
	ServiceLocator& operator=(const ServiceLocator&) = delete;
	ServiceLocator(const ServiceLocator&&) = delete;
	ServiceLocator& operator=(ServiceLocator&&) = delete;

	/**
	 * @brief Registers a service instance for type `T`.
	 * @tparam T the service type (interface or concrete class).
	 * @param service Shared pointer to the service instance.
	 * @throw None (guaranteed not to throw due to lock_guard and
	 * map operation).
	 * @note Overwrites any existing registeration for `T`.
	 */
	template<typename T>
	static void register_service(std::shared_ptr<T> service)
	{
		std::lock_guard<std::mutex> lock(get_mutex());
		auto& services = get_services();
		services[typeid(T)] = service;
	}

	/**
	 * @brief Registers a factory function for lazy instantiation of service `T`.
	 * @tparam T the service type.
	 * @param factory Factory function returning `std::shared_pointer<T>`.
	 * @throw None (thread-safe and noexcept).
	 * @note The factory is invoked only upon the first `resolve<T>()` call.
	 */
	template<typename T>
	static void register_service_factory(std::function<std::shared_ptr<T>()> factory)
	{
		std::lock_guard<std::mutex> lock(get_mutex());
		auto& factories = get_factories();
		factories[typeid(T)] = [factory]() -> std::shared_ptr<void> { return factory(); };
	}

	/**
     * @brief Resolves and returns a service instance of type `T`.
     * @tparam T The service type to resolve.
     * @return std::shared_ptr<T> Shared pointer to the service instance.
     * @throws std::runtime_error If no service or factory is registered for `T`.
     * @note (Thread-safe), blocks until registration/factory completion.
     */
	template<typename T>
	[[nodiscard]] static std::shared_ptr<T> resolve()
	{
		std::lock_guard<std::mutex> lock(get_mutex());

		// First checking service instance
		auto& services = get_services();
		auto it = services.find(typeid(T));
		if (it != services.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}

		// Then checking factory instace
		auto& factories = get_factories();
		auto factory_it = factories.find(typeid(T));
		if (factory_it != factories.end())
		{
			auto factory = factory_it->second;
			auto instance = factory(nullptr);
			services[typeid(T)] = instance;
			return std::static_pointer_cast<T>(instance);
		}

		throw std::runtime_error("Service not registered: " + 
								  std::string(typeid(T).name()));
	}

	/**
	 * @brief Checks if a service of type `T` is registered.
	 * @tparam T The service type to check.
	 * @return bool The if either ann instance or factory exists for `T`.
	 */
	template<typename T>
	[[nodiscard]] static bool is_registered() noexcept
	{
		std::lock_guard<std::mutex> lock(get_mutex());
		auto& services = get_services();
		auto& factories = get_factories();
		return services.find(typeid(T)) != services.end() ||
			   factories.find(typeid(T)) != factories.end();
	}

	/**
     * @brief Removes all registrations (instance and factory) for type `T`.
     * @tparam T The service type to unregister.
     * @throws None (thread-safe).
     * @note Safe to call even if `T` is not registered.
     */
    template<typename T>
    static void unregister_service() noexcept
    {
        std::lock_guard<std::mutex> lock(get_mutex());
        get_services().erase(typeid(T));
        get_factories().erase(typeid(T));
    }

	/**
     * @brief Clears all registered services and factories.
     * @throws None (thread-safe).
     * @warning Invalidates all existing shared_ptr references 
     * returned by `resolve()`.
     */
	static void clear() noexcept
	{
		std::lock_guard<std::mutex> lock(get_mutex());
		get_services().clear();
		get_factories().clear();
	}
private:
	// Type aliases of internal storage.
	using ServiceMap = std::unordered_map<std::type_index, std::shared_ptr<void>>;
	using FactoryMap = 
		std::unordered_map<std::type_index, 
						   std::function<std::shared_ptr<void>(void*)>>;

	/**
     * @brief Returns the singleton service instance map.
     * @return ServiceMap& Mutable reference to the static service map.
     * @details Initialized on first use (Meyer's Singleton).
     */
	static ServiceMap& get_services()
	{
		static ServiceMap instance;
		return instance;
	}

	/**
     * @brief Returns the singleton factory map.
     * @return FactoryMap& Mutable reference to the static factory map.
     */
	static FactoryMap& get_factories()
	{
		static FactoryMap instance;
		return instance;
	}

	/**
     * @brief Returns the mutex for thread synchronization.
     * @return std::mutex& Static mutex instance.
     */
	static std::mutex& get_mutex()
	{
		static std::mutex mutex;
		return mutex;
	}
};

} // namespace RoboTact::Core
#endif // SERVICE_LOCATOR_HPP