#pragma once
#include <entt/entt.hpp>
#include "jam/core/clock.hpp"
#include <memory>
#include <vector>

namespace jam
{
	class iSystem
	{
	public:
		virtual ~iSystem() {}

		virtual void onSceneStart(entt::registry& world) {}
		virtual void onSceneEnd(entt::registry& world) {};
		virtual void onPollEvent(entt::registry& world) {};
		virtual void onUpdate(entt::registry& world, const Clock& time) {};
		virtual void onFixedUpdate(entt::registry& world, const Clock& time) {};
		//rendering functions
		virtual void onRenderWorld(entt::registry& world) {};
		virtual void onRenderUI(entt::registry& world) {};
	};


	class SystemManager {
	public:
		SystemManager();
		~SystemManager();
		void clear() { m_system_list.clear(); }
#if false
		template<typename TSystem, typename... Args>
		requires(std::is_base_of_v<iSystem, TSystem>)
		TSystem* pushSystem(Args&&... args) 
		{
			auto& system = m_system_list.emplace_back(std::make_unique<TSystem>(std::forward<Args>(args)...);
			return dynamic_cast<TSystem*>(system.get());
		}
#else

		template<typename TLayer>
			requires(std::is_base_of_v<iSystem, TLayer>)
		void pushSystem()
		{
			m_system_list.push_back(std::make_unique<TLayer>());
		}
#endif

	public:
		//lifetime methods
		void onSceneStart(entt::registry& world);
		void onSceneEnd(entt::registry& world);
		void onPollEvent(entt::registry& world);
		void onUpdate(entt::registry& world, const Clock& time);
		void onFixedUpdate(entt::registry& world, const Clock& time);
		void onRenderWorld(entt::registry& world);
		void onRenderUI(entt::registry& world);
	private:
		std::vector<std::unique_ptr<jam::iSystem>> m_system_list;
	};

}