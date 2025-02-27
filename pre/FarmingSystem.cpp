// FarmingSystem.cpp
#include <entt/entt.hpp>
#include <components.hpp>

struct HarvestEvent
{
    entt::entity target;
};

class FarmingSystem
{
public:
    void update(entt::registry &registry, float dt)
    {
        auto view = registry.view<Transform, Soil, Crop>();
        for (auto entity : view)
        {
            auto &soil = view.get<Soil>(entity);
            auto &crop = view.get<Crop>(entity);

            if (soil.fertile)
            {
                // 依据天气、季节等因素影响生长速度
                crop.growth += dt * 0.1f * getWeatherFactor(); // 添加天气因子

                if (crop.growth >= 1.0f)
                {
                    // 触发收获事件
                    registry.emplace_or_replace<HarvestEvent>(entity);
                }
            }
        }
    }

private:
    float getWeatherFactor()
    {
        // 返回当前天气对作物生长的影响因子，可以在以后扩展
        return 1.0f; // 示例：可以增加季节、天气等影响因素
    }
};
