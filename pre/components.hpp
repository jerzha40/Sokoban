#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
// components.hpp
#include <entt/entt.hpp>
#include <glm/glm.hpp>

//------------------
// 基础组件
//------------------
struct Transform
{
    glm::ivec2 gridPos; // 网格坐标 (如 [3,5])
    glm::vec2 worldPos; // 实际渲染坐标
};

struct Sprite
{
    glm::vec3 color;
    glm::vec2 size;
};

struct Movable
{
    bool canPush = false; // 是否可被推动（箱子）
    bool isPlayer = false;
};

struct GridMap
{
    int width;
    int height;
    std::vector<char> tiles; // 存储地图数据 '#', '@', '$', '.' 等
};

//------------------
// 种地系统组件
//------------------
struct Soil
{
    bool fertile = true;
    bool occupied = false;
};

struct Crop
{
    enum Type
    {
        Wheat,
        Carrot,
        Potato
    } type;
    float growth = 0.0f; // 0~1 生长进度
};

//------------------
// 战斗系统组件
//------------------
struct CombatStats
{
    int health = 100;
    int attack = 20;
    int defense = 10;
};

struct Enemy
{
    enum State
    {
        Idle,
        Patrol,
        Chase
    } state;
};

//------------------
// 探索系统组件
//------------------
struct FogOfWar
{
    std::vector<bool> revealed;
    int width, height;

    FogOfWar(int w, int h) : width(w), height(h), revealed(w * h, false) {}
};

struct Discoverable
{
    std::string hint;
    bool triggered = false;
};
#endif
