#include <rad/Scene/Scene.test.h>

#include <entt/entity/registry.hpp>

// https://github.com/skypjack/entt#code-example
TEST(Entity, Basics)
{
    struct Position
    {
        float x;
        float y;
    };

    struct Velocity
    {
        float dx;
        float dy;
    };

    entt::registry registry;

    for (unsigned i = 0u; i < 10u; ++i)
    {
        const entt::entity entity = registry.create();
        registry.emplace<Position>(entity, i * 1.f, i * 1.f);
        if (i % 2u == 0u)
        {
            registry.emplace<Velocity>(entity, i * .1f, i * .1f);
        }
    }

    const auto view = registry.view<const Position, Velocity>();
    EXPECT_EQ(view.size_hint(), 5u);
    for (auto [entity, pos, vel] : view.each())
    {
        EXPECT_TRUE((registry.all_of<Position, Velocity>(entity)));
    }
}

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
