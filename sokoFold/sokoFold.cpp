#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <components.h>
#include <iostream>
void MovementSystem(entt::registry &registry, GLFWwindow *window)
{
    static bool lastUp = false, lastDown = false, lastLeft = false, lastRight = false;

    auto view = registry.view<components::Transform, components::Player>();

    for (auto entity : view)
    {
        auto &transform = view.get<components::Transform>(entity);

        // 获取按键状态
        bool currentUp = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        bool currentDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        bool currentLeft = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        bool currentRight = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

        int dx = 0, dy = 0;

        // **仅在按键“刚被按下”的瞬间触发移动**
        if (currentUp && !lastUp)
            dy = -1;
        if (currentDown && !lastDown)
            dy = 1;
        if (currentLeft && !lastLeft)
            dx = -1;
        if (currentRight && !lastRight)
            dx = 1;

        // 记录本帧的按键状态，供下次帧检测
        lastUp = currentUp;
        lastDown = currentDown;
        lastLeft = currentLeft;
        lastRight = currentRight;

        if (dx == 0 && dy == 0)
            return; // 没有按键按下，直接返回

        int newX = static_cast<int>(transform.position.x / 50.0f) + dx;
        int newY = static_cast<int>(transform.position.y / 50.0f) + dy;
        bool canMove = true;

        // **检测是否撞墙**
        auto wallView = registry.view<components::Transform, components::Wall>();
        for (auto wallEntity : wallView)
        {
            auto &wallTransform = wallView.get<components::Transform>(wallEntity);
            if (static_cast<int>(wallTransform.position.x / 50.0f) == newX &&
                static_cast<int>(wallTransform.position.y / 50.0f) == newY)
            {
                canMove = false;
            }
        }

        // **检测箱子碰撞**
        auto boxView = registry.view<components::Transform, components::Box>();
        for (auto boxEntity : boxView)
        {
            auto &boxTransform = boxView.get<components::Transform>(boxEntity);
            if (static_cast<int>(boxTransform.position.x / 50.0f) == newX &&
                static_cast<int>(boxTransform.position.y / 50.0f) == newY)
            {
                int boxNewX = newX + dx;
                int boxNewY = newY + dy;
                bool boxCanMove = true;

                for (auto otherBox : boxView)
                {
                    auto &otherBoxTransform = boxView.get<components::Transform>(otherBox);
                    if (static_cast<int>(otherBoxTransform.position.x / 50.0f) == boxNewX &&
                        static_cast<int>(otherBoxTransform.position.y / 50.0f) == boxNewY)
                    {
                        boxCanMove = false;
                    }
                }
                for (auto wallEntity : wallView)
                {
                    auto &wallTransform = wallView.get<components::Transform>(wallEntity);
                    if (static_cast<int>(wallTransform.position.x / 50.0f) == boxNewX &&
                        static_cast<int>(wallTransform.position.y / 50.0f) == boxNewY)
                    {
                        boxCanMove = false;
                    }
                }

                if (boxCanMove)
                {
                    boxTransform.position.x += dx * 50.0f;
                    boxTransform.position.y += dy * 50.0f;
                }
                else
                {
                    canMove = false;
                }
            }
        }

        if (canMove)
        {
            transform.position.x += dx * 50.0f;
            transform.position.y += dy * 50.0f;
        }
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
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // Windows 清屏命令
#else
    system("clear"); // Linux / macOS 清屏命令
#endif
}
void TerminalRenderSystem(entt::registry &registry)
{
    auto view = registry.view<components::Level>();
    for (auto entity : view)
    {
        auto &levelComponent = view.get<components::Level>(entity);

        // **1. 复制 levelData 作为基础**
        std::vector<std::string> renderedMap = levelComponent.levelData;

        // **2. 清除旧的 `@` 和 `$`，避免残影**
        for (auto &row : renderedMap)
        {
            for (char &cell : row)
            {
                if (cell == '@' || cell == '$')
                {
                    cell = ' '; // 变为空格，清除旧位置
                }
            }
        }

        // **3. 遍历 ECS 里的实体，重新绘制地图**

        // **画墙壁**
        auto wallView = registry.view<components::Transform, components::Wall>();
        for (auto wallEntity : wallView)
        {
            auto &transform = wallView.get<components::Transform>(wallEntity);
            int x = static_cast<int>(transform.position.x / 50.0f);
            int y = static_cast<int>(transform.position.y / 50.0f);
            renderedMap[y][x] = '#';
        }

        // **画目标点（.）**
        auto goalView = registry.view<components::Transform, components::Goal>();
        for (auto goalEntity : goalView)
        {
            auto &transform = goalView.get<components::Transform>(goalEntity);
            int x = static_cast<int>(transform.position.x / 50.0f);
            int y = static_cast<int>(transform.position.y / 50.0f);
            renderedMap[y][x] = '.';
        }

        // **画箱子**
        auto boxView = registry.view<components::Transform, components::Box>();
        for (auto boxEntity : boxView)
        {
            auto &transform = boxView.get<components::Transform>(boxEntity);
            int x = static_cast<int>(transform.position.x / 50.0f);
            int y = static_cast<int>(transform.position.y / 50.0f);
            renderedMap[y][x] = '$';
        }

        // **画玩家**
        auto playerView = registry.view<components::Transform, components::Player>();
        for (auto playerEntity : playerView)
        {
            auto &transform = playerView.get<components::Transform>(playerEntity);
            int x = static_cast<int>(transform.position.x / 50.0f);
            int y = static_cast<int>(transform.position.y / 50.0f);
            renderedMap[y][x] = '@';
        }

        // **4. 清屏**
        clearScreen();

        // **5. 输出渲染后的地图**
        for (const auto &line : renderedMap)
        {
            std::cout << line << std::endl;
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
    // 清空所有旧实体
    registry.clear();

    auto levelEntity = registry.create();
    registry.emplace<components::Level>(levelEntity, levels[levelIndex]);

    // 遍历地图并创建对应的实体
    for (int y = 0; y < levels[levelIndex].size(); ++y)
    {
        for (int x = 0; x < levels[levelIndex][y].size(); ++x)
        {
            char tile = levels[levelIndex][y][x];

            if (tile == '@')
            {
                // 玩家
                auto playerEntity = registry.create();
                registry.emplace<components::Transform>(playerEntity, glm::vec2(x * 50.0f, y * 50.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(playerEntity, "playerTexture.png");
                registry.emplace<components::Player>(playerEntity);
            }
            else if (tile == '#')
            {
                // **墙壁**
                auto wallEntity = registry.create();
                registry.emplace<components::Transform>(wallEntity, glm::vec2(x * 50.0f, y * 50.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(wallEntity, "wallTexture.png");
                registry.emplace<components::Wall>(wallEntity); // ✅ **正确创建墙壁**
            }
            else if (tile == '$')
            {
                // 箱子
                auto boxEntity = registry.create();
                registry.emplace<components::Transform>(boxEntity, glm::vec2(x * 50.0f, y * 50.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(boxEntity, "crateTexture.png");
                registry.emplace<components::Box>(boxEntity);
            }
            else if (tile == '.')
            {
                // 目标点
                auto goalEntity = registry.create();
                registry.emplace<components::Transform>(goalEntity, glm::vec2(x * 50.0f, y * 50.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(goalEntity, "goalTexture.png");
                registry.emplace<components::Goal>(goalEntity);
            }
        }
    }
}
#include <set>
// 检查是否所有目标点 `.` 上都有箱子 `$`
bool CheckLevelComplete(entt::registry &registry)
{
    auto view = registry.view<components::Level>();
    for (auto entity : view)
    {
        auto &levelComponent = view.get<components::Level>(entity);
        std::vector<std::string> &levelData = levelComponent.levelData;

        // 1. 统计所有目标点 `.` 的坐标
        std::set<std::pair<int, int>> goalPositions;
        for (int y = 0; y < levelData.size(); ++y)
        {
            for (int x = 0; x < levelData[y].size(); ++x)
            {
                if (levelData[y][x] == '.')
                {
                    goalPositions.insert({x, y});
                }
            }
        }

        // 2. 遍历所有 `Box` 组件，记录箱子覆盖的位置
        std::set<std::pair<int, int>> coveredGoals;
        auto boxView = registry.view<components::Transform, components::Box>();
        for (auto boxEntity : boxView)
        {
            auto &transform = boxView.get<components::Transform>(boxEntity);
            int x = static_cast<int>(transform.position.x / 50.0f);
            int y = static_cast<int>(transform.position.y / 50.0f);
            coveredGoals.insert({x, y});
        }

        // 3. 检查所有 `.` 是否都被 `$` 覆盖
        for (const auto &goal : goalPositions)
        {
            if (coveredGoals.find(goal) == coveredGoals.end())
            {
                return false; // 有 `.` 没有被 `$` 覆盖，关卡未完成
            }
        }

        return true; // 所有 `.` 都被 `$` 覆盖，关卡完成！
    }
    return false;
}
#include <iostream>
#include <chrono>
#include <thread>
int main()
{
    entt::registry registry;
    int currentLevel = 0;

    // 初始化GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(100, 100, std::string("SokobanGame").c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    auto levelEntity = registry.create();
    registry.emplace<components::Level>(levelEntity);

    LoadLevelData(registry, 0);
    const std::chrono::milliseconds dt(100); // 100ms = 0.1秒

    while (!glfwWindowShouldClose(window))
    {
        auto start = std::chrono::steady_clock::now();

        glfwPollEvents();
        // 处理玩家移动
        MovementSystem(registry, window);

        // 终端渲染
        TerminalRenderSystem(registry);

        // **检测是否过关**
        if (CheckLevelComplete(registry))
        {
            std::cout << "Level" << currentLevel + 1 << " fin!" << std::endl;

            // 切换到下一关
            currentLevel++;
            if (currentLevel >= levels.size())
            {
                std::cout << "congradulation you have done all the levels" << std::endl;
                break;
            }
            else
            {
                std::cout << "level " << currentLevel + 1 << " loading..." << std::endl;
                LoadLevelData(registry, currentLevel);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        clearScreen();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
