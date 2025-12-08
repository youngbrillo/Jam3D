#include "system.hpp"

jam::SystemManager::SystemManager()
{
}

jam::SystemManager::~SystemManager()
{
}

void jam::SystemManager::onSceneStart(entt::registry& world){
	for (auto& system : m_system_list) system->onSceneStart(world);
}

void jam::SystemManager::onSceneEnd(entt::registry& world)
{
	for (auto& system : m_system_list) system->onSceneEnd(world);

}

void jam::SystemManager::onPollEvent(entt::registry& world)
{
	for (auto& system : m_system_list) system->onPollEvent(world);
}

void jam::SystemManager::onUpdate(entt::registry& world, const Clock& time)
{
	for (auto& system : m_system_list) system->onUpdate(world, time);
}

void jam::SystemManager::onFixedUpdate(entt::registry& world, const Clock& time)
{
	for (auto& system : m_system_list) system->onFixedUpdate(world, time);
}

void jam::SystemManager::onRenderWorld(entt::registry& world)
{
	for (auto& system : m_system_list) system->onRenderWorld(world);
}

void jam::SystemManager::onRenderUI(entt::registry& world)
{
	for (auto& system : m_system_list) system->onRenderUI(world);
}
