// CombatSystem.cpp
#include <entt/entt.hpp>
#include <components.hpp>
#include <unordered_map>

class CombatSystem
{
public:
    void update(entt::registry &registry)
    {
        auto fighters = registry.view<Transform, CombatStats>();

        // 使用空间分区：将所有单位按位置存储，减少遍历
        std::unordered_map<int, std::vector<entt::entity>> grid;
        for (auto entity : fighters)
        {
            auto &transform = fighters.get<Transform>(entity);
            int gridPos = transform.gridPos.x + transform.gridPos.y * 1000; // 假设最大地图大小是1000x1000
            grid[gridPos].push_back(entity);
        }

        // 处理战斗
        for (auto &[pos, entities] : grid)
        {
            for (auto attacker : entities)
            {
                // 将整数类型位置转换为浮动类型
                glm::vec2 attackerPos = glm::vec2(fighters.get<Transform>(attacker).gridPos);
                for (auto target : entities)
                {
                    if (attacker == target)
                        continue;

                    // 将目标位置转换为浮动类型
                    glm::vec2 targetPos = glm::vec2(fighters.get<Transform>(target).gridPos);

                    if (glm::distance(attackerPos, targetPos) < 2.0f)
                    {
                        auto &aStats = fighters.get<CombatStats>(attacker);
                        auto &tStats = fighters.get<CombatStats>(target);
                        int damage = std::max(0, aStats.attack - tStats.defense); // 确保伤害为非负数
                        tStats.health -= damage;

                        if (tStats.health <= 0)
                        {
                            registry.destroy(target);
                        }
                    }
                }
            }
        }
    }
};
