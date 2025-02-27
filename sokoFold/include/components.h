#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <glm/glm.hpp>
#include <string>
#include <vector>
namespace components
{
    struct Transform
    {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 velocity;
    };
    struct Render
    {
        std::string textureName;
        glm::vec3 colorTint = glm::vec3(1.0f); // 可选的颜色调色
    };
    struct Player
    {
        bool isPlayer = true;
    };
    struct Movement
    {
        int dx = 0;
        int dy = 0;
    };
    struct Level
    {
        std::vector<std::string> levelData;
        int currentLevel = 0;
    };
    struct Box
    {
        /* data */
    };
    struct Wall
    {
        /* data */
    };
    struct Goal
    {
        /* data */
    };
} // namespace components
#endif
