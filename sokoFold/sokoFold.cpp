#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <components.h>
void MovementSystem(entt::registry &registry, GLFWwindow *window)
{
    auto view = registry.view<components::Transform, components::Movement>();

    for (auto entity : view)
    {
        auto &transform = view.get<components::Transform>(entity);
        auto &movement = view.get<components::Movement>(entity);

        // 获取按键状态
        bool currentUp = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        bool currentDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        bool currentLeft = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        bool currentRight = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

        // 更新移动
        if (currentUp)
            movement.dy = -1;
        if (currentDown)
            movement.dy = 1;
        if (currentLeft)
            movement.dx = -1;
        if (currentRight)
            movement.dx = 1;

        // 更新实体的位置
        transform.position.x += movement.dx;
        transform.position.y += movement.dy;
    }
}
#include <renderer.h>
void RenderSystem(entt::registry &registry, Renderer &renderer)
{
    auto view = registry.view<components::Transform, components::Render>();

    for (auto entity : view)
    {
        auto &transform = view.get<components::Transform>(entity);
        auto &render = view.get<components::Render>(entity);

        // 计算物体的显示位置和大小
        glm::vec2 position = transform.position;
        glm::vec2 size = transform.size;

        // 绘制物体
        renderer.DrawQuad(position, size, render.textureName, render.colorTint);
    }
}
#include <iostream>
#include <format>
void TerminalRenderSystem(entt::registry &registry)
{
    auto view = registry.view<components::Level>();
    for (auto &&entiti : view)
    {
        auto &vel = view.get<components::Level>(entiti);
        for (auto &&str : vel.levelData)
        {
            std::cout << std::format("{}\n", str);
        }
    }
}
std::vector<std::vector<std::string>> levels = {{// Level 0
                                                 "#######",
                                                 "#     #",
                                                 "#  $  #",
                                                 "#  @  #",
                                                 "#  .  #",
                                                 "#     #",
                                                 "#######"},
                                                {// Level 1
                                                 "########",
                                                 "#.  @  #",
                                                 "# $$   #",
                                                 "#      #",
                                                 "########"},
                                                {// Level 2 (更复杂的关卡)
                                                 "##########",
                                                 "#.$ #  @ #",
                                                 "#   $   $#",
                                                 "##########"}};
void LoadLevelData(entt::registry &registry, int levelIndex)
{
    auto view = registry.view<components::Level>();

    for (auto entity : view)
    {
        auto &levelComponent = view.get<components::Level>(entity);
        levelComponent.levelData = levels[levelIndex];
    }
}
#include <iostream>
#include <chrono>
#include <thread>

void clearScreen()
{
#ifdef _WIN32
    system("cls"); // Windows 清屏命令
#else
    system("clear"); // Linux / macOS 清屏命令
#endif
}
int main()
{
    entt::registry registry;

    // 创建游戏实体
    auto playerEntity = registry.create();
    registry.emplace<components::Transform>(playerEntity, glm::vec2(100.0f, 100.0f), glm::vec2(50.0f, 50.0f));
    registry.emplace<components::Render>(playerEntity, "playerTexture.png");
    registry.emplace<components::Player>(playerEntity);

    auto levelEntity = registry.create();
    registry.emplace<components::Level>(levelEntity);

    LoadLevelData(registry, 0);
    const std::chrono::milliseconds dt(100); // 100ms = 0.1秒

    // 游戏循环
    while (true)
    {
        auto start = std::chrono::steady_clock::now();

        // 渲染游戏内容
        TerminalRenderSystem(registry);

        // 等待下一帧
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        auto sleep_time = dt - duration; // 计算剩余时间以保持固定帧率
        if (sleep_time > std::chrono::milliseconds(0))
        {
            std::this_thread::sleep_for(sleep_time); // 等待直到下一帧
        }
        // 清屏
        clearScreen();
    }
    return 0;
}
