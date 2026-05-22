#include <rad/Scene/Scene.h>

namespace rad
{

Scene::Scene() = default;

Entity Scene::CreateEntity()
{
    return m_registry.create();
}

void Scene::DestroyEntity(Entity entity)
{
    if (entity == NullEntity)
    {
        return;
    }
    m_registry.destroy(entity);
}

bool Scene::IsAlive(Entity entity) const
{
    return entity != NullEntity && m_registry.valid(entity);
}

void Scene::Clear()
{
    m_registry.clear();
}

} // namespace rad
