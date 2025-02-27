// ExplorationSystem.cpp
#include <entt/entt.hpp>
#include <components.hpp>

class ExplorationSystem
{
public:
    void update(entt::registry &registry)
    {
        // 获取玩家位置
        auto playerView = registry.view<const Movable, const Transform>();
        glm::ivec2 playerPos{0, 0};
        for (auto entity : playerView)
        {
            if (playerView.get<Movable>(entity).isPlayer)
            {
                playerPos = playerView.get<Transform>(entity).gridPos;
                break;
            }
        }

        // 更新战争迷雾
        auto mapView = registry.view<GridMap, FogOfWar>();
        for (auto mapEntity : mapView)
        {
            auto &fog = mapView.get<FogOfWar>(mapEntity);
            const auto &map = mapView.get<GridMap>(mapEntity);

            // 可视范围
            for (int y = -3; y <= 3; ++y)
            {
                for (int x = -3; x <= 3; ++x)
                {
                    int idx = (playerPos.y + y) * map.width + (playerPos.x + x);
                    if (idx >= 0 && idx < fog.revealed.size())
                    {
                        fog.revealed[idx] = true;
                    }
                }
            }
        }

        // 发现物体检测
        auto discoverableView = registry.view<Transform, Discoverable>();
        for (auto entity : discoverableView)
        {
            auto pos = discoverableView.get<Transform>(entity).gridPos;

            // 将整数类型的 pos 转换为浮动类型
            glm::vec2 playerPosFloat = glm::vec2(playerPos);
            glm::vec2 entityPosFloat = glm::vec2(pos);

            if (glm::distance(playerPosFloat, entityPosFloat) < 2.0f)
            {
                auto &discoverable = discoverableView.get<Discoverable>(entity);
                discoverable.triggered = true;
            }
        }
    }
};
