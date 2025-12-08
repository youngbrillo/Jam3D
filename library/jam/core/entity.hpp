#pragma once
#ifndef JAM_ENTITY_HPP
#define JAM_ENTITY_HPP
#endif // !JAM_ENTITY_HPP

#include "uuid.hpp"
#include <string>
#include <entt/entt.hpp>
 // #include <raylib.h>
#ifdef JAM_ENTITY_HPP

struct Vector3;
struct Matrix;

 namespace jam
 {
 	namespace components
 	{
 		struct NameTag {
 			std::string name = "entity";
 		};
 		struct IDTag { UUID tag = 0; };
 		struct HiddenTag { char x; };
 		struct DisabledTag { char x; };
 		struct DeleteTag { char x; };
 		struct DoNotSerializeTag { char x; };
        struct HasChildrenTag { uint32_t count = 0; };
        struct Relationship
        {
            entt::entity parent{ entt::null };
            entt::entity firstChild{ entt::null };
            entt::entity prevSib{ entt::null };
            entt::entity nextSib{ entt::null };
        };
 	}
 	class Scene;

 	class Entity
 	{
 	public:
 		Entity() = default;
 		Entity(entt::entity handle, entt::registry& regsitry);
 		Entity(entt::entity handle, entt::registry* regsitry);
 		Entity(const Entity&) = default;

 	public:
 		/// @brief check if entity has component 'T'
 		/// @tparam T : component
 		/// @return : returns true if entity contains T
 		template <typename T>
 		inline bool has();

 		/// @brief Add Component 'T' to entity. Will throw if entity already possesses the component
 		/// @tparam T 
 		/// @tparam ...Args constructor arguments
 		/// @param ...args 
 		/// @return 
 		template <typename T, typename... Args>
 		inline T& add(Args&&... args);
        
 		/// @brief Add Component 'T' to enitty. Will not throw if entity already possesses the component
 		/// @tparam T 
 		/// @tparam ...Args 
 		/// @param ...args 
 		/// @return 
 		template <typename T, typename... Args>
 		inline T& addOrReplace(Args&&... args);

 		/// @brief Get Component 'T', Will throw if enitty does not possess the component.
 		/// @tparam T 
 		/// @return 
 		template <typename T>
 		inline T& get();

 		/// @brief Try to Get Component 'T'. 
 		/// @tparam T 
 		/// @return retuns a pointer to the component if found, else returns a nullptr
 		template <typename T>
 		inline T* tryGet();

 		/// @brief remove a compnent of type 'T'. Will not throw if if component is not found
 		/// @tparam T 
 		template <typename T>
 		inline void remove();

 		/// @brief Destroy an entity and all it's children (recursive). Unparents entity from parent.
 		void destroy();
 	public:
 		std::string getName();
 		UUID getUUID();

        Vector3 getLocalPosition();
        Vector3 getWorldPosition();

        Matrix GetLocalMatrix();
        Matrix GetWorldMatrix();

 		 void setPosition(Vector3 position);

 		bool isEnabled();
 		void enable();
 		void disable();

 		bool isVisible();
 		void show();
 		void hide();

 		bool isSerializable();
 		void disableSerialization();
 		void enableSerialization();

 	public:
 		/// @brief creates an entity with a nameTag, UUIDTag, and Relationship Components
 		/// @return 
 		Entity createEntity();
 		/// @brief work in progress
 		/// @param scene 
 		/// @return 
 		// Entity duplicate(iScene* scene);

 		/// @brief creates an entity with a nameTag, UUIDTag, and Relationship Components. Parents the new component.
 		/// @param name 
 		/// @return 
 		Entity createChild(std::string name);

 		void addParent(Entity parent);
 		void removeParent();
 		void addChild(Entity child);
 		void removeChild(Entity child);
 		void removeAllChildren();
 		Entity getFirstChild();
 		Entity getNextSibling();
 		Entity getPrevSibling();
 		Entity getParent();
 		/// @brief checks if the entity provided is a child of the current entity.
 		/// @param child 
 		/// @return 
 		bool hasChild(Entity child);     

 		/// @brief checks if this entity is decendant of the provided entity.
 		/// @param ancestor 
 		/// @return 
 		bool isDecendantOf(Entity ancestor);

 		/// @brief returns true if the entity has a parent
 		/// @return 
 		bool hasParent();
        
 		/// @brief returns number of children
 		/// @return 
 		uint32_t getNumberOfChildren();

		entt::registry* getWorld() {return mWorld;}
 	public:

 		bool isValid() const { return mHandle != entt::null && mWorld != nullptr; }
 		uint32_t handle() const { return (uint32_t)mHandle; }

 		operator bool() const { return mHandle != entt::null && mWorld != nullptr; }
 		operator entt::entity() const { return mHandle; }
 		operator uint32_t() const { return (uint32_t)mHandle; }
 	private:
 		entt::entity mHandle = { entt::null };
 		entt::registry* mWorld = { 0 };
 	public:

 		bool operator==(const Entity& other) const
 		{
 			return mHandle == other.mHandle && mWorld == other.mWorld;
 		}

 		bool operator!=(const Entity& other) const
 		{
 			return !(*this == other);
 		}
 	};
 }

 namespace jam
 {
 	template<typename T>
 	inline bool Entity::has()
 	{
 		assert(mHandle != entt::null && "Entity handle cannot be null!");
 		assert(mWorld != NULL && "Registry cannot be invalid!");

 		return mWorld->try_get<T>(mHandle) != nullptr;
 	}
 	template<typename T>
 	inline T& Entity::get()
 	{
 		assert(mHandle != entt::null && "Entity handle cannot be null!");
 		assert(mWorld != NULL && "Registry cannot be invalid!");

 		return mWorld->get<T>(mHandle);
 	}
 	template<typename T>
 	inline T* Entity::tryGet()
 	{
 		assert(mHandle != entt::null && "Entity handle cannot be null!");
 		assert(mWorld != NULL && "Registry cannot be invalid!");

 		return mWorld->try_get<T>(mHandle);
 	}
 	template<typename T, typename ...Args>
 	inline T& Entity::add(Args && ...args)
 	{
 		assert(mHandle != entt::null && "Entity handle cannot be null!");
 		assert(mWorld != NULL && "Registry cannot be invalid!");

 		return mWorld->emplace<T>(mHandle, std::forward<Args>(args)...);
 	}
 	template<typename T, typename ...Args>
 	inline T& Entity::addOrReplace(Args && ...args)
 	{
 		assert(mHandle != entt::null && "Entity handle cannot be null!");
 		assert(mWorld != NULL && "Registry cannot be invalid!");

 		return mWorld->emplace_or_replace<T>(mHandle, std::forward<Args>(args)...);
 		// TODO: insert return statement here
 	}
 	template<typename T>
 	inline void Entity::remove()
 	{
 		if (has<T>())
 		{
 			mWorld->remove<T>(mHandle);
 		}
 	}
 }
#endif