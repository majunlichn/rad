#pragma once

#include <rad/Scene/Entity.h>

#include <entt/entity/registry.hpp>

#include <utility>

namespace rad
{

// CPU-side scene model backed by EnTT. Rendering and physics read components via the registry.
class Scene
{
public:
    Scene();
    ~Scene() = default;

    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    Scene(Scene&&) noexcept = default;
    Scene& operator=(Scene&&) noexcept = default;

    [[nodiscard]] Entity CreateEntity();
    void DestroyEntity(Entity entity);
    [[nodiscard]] bool IsAlive(Entity entity) const;

    void Clear();

    [[nodiscard]] entt::registry& GetRegistry() noexcept { return m_registry; }
    [[nodiscard]] const entt::registry& GetRegistry() const noexcept { return m_registry; }

    template <typename Component, typename... Args>
    Component& Emplace(Entity entity, Args&&... args)
    {
        return m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component, typename... Args>
    Component& EmplaceOrReplace(Entity entity, Args&&... args)
    {
        return m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
    }

    template <typename Component>
    [[nodiscard]] Component& Get(Entity entity)
    {
        return m_registry.get<Component>(entity);
    }

    template <typename Component>
    [[nodiscard]] const Component& Get(Entity entity) const
    {
        return m_registry.get<Component>(entity);
    }

    template <typename Component>
    [[nodiscard]] bool Has(Entity entity) const
    {
        return m_registry.all_of<Component>(entity);
    }

    template <typename Component>
    void Remove(Entity entity)
    {
        m_registry.remove<Component>(entity);
    }

private:
    entt::registry m_registry;
    
}; // class Scene

} // namespace rad
