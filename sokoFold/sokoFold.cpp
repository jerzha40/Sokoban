#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <entt/entt.hpp>
#include <components.h>
#include <iostream>
#include <unordered_map>
#include <iostream>
#include <format>
std::unordered_map<int, bool> g_KeyState;
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
float tileSize = 100.0f;
void LevelAnimationSystem(entt::registry &registry, float t, float dt)
{
    auto goalView = registry.view<components::Goal>();
    for (auto entity : goalView)
    {
        auto &Tras = registry.get<components::Transform>(entity);
        // std::cout << std::format("{} {}\n", Tras.size.x, Tras.size.y);
        Tras.size *= exp(0.2 * sin(t) * dt);
    }
}
void MovementSystem(entt::registry &registry)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);
        if (g_KeyState[GLFW_KEY_ESCAPE])
        {
            game.state = components::GameState::LevelSelect;
            g_KeyState[GLFW_KEY_ESCAPE] = false;
            return;
        }
    }

    auto view = registry.view<components::Transform, components::Player>();

    for (auto entity : view)
    {
        auto &transform = view.get<components::Transform>(entity);

        int dx = 0, dy = 0;

        // **使用 `g_KeyState` 处理移动逻辑**
        if (g_KeyState[GLFW_KEY_UP])
        {
            dy = -1;
            g_KeyState[GLFW_KEY_UP] = false; // 防止连续触发
        }
        if (g_KeyState[GLFW_KEY_DOWN])
        {
            dy = +1;
            g_KeyState[GLFW_KEY_DOWN] = false;
        }
        if (g_KeyState[GLFW_KEY_LEFT])
        {
            dx = -1;
            g_KeyState[GLFW_KEY_LEFT] = false;
        }
        if (g_KeyState[GLFW_KEY_RIGHT])
        {
            dx = 1;
            g_KeyState[GLFW_KEY_RIGHT] = false;
        }

        if (dx == 0 && dy == 0)
            return; // 没有按键按下，直接返回

        int newX = transform.gridpos.x + dx;
        int newY = transform.gridpos.y + dy;
        std::cout << std::format("({},{})({},{})\n", newX, newY, transform.gridpos.x, transform.gridpos.y);
        bool canMove = true;

        // **检测是否撞墙**
        auto wallView = registry.view<components::Transform, components::Wall>();
        for (auto wallEntity : wallView)
        {
            auto &wallTransform = wallView.get<components::Transform>(wallEntity);
            if (wallTransform.gridpos.x == newX &&
                wallTransform.gridpos.y == newY)
            {
                canMove = false;
            }
        }

        // **检测箱子碰撞**
        auto boxView = registry.view<components::Transform, components::Box>();
        for (auto boxEntity : boxView)
        {
            auto &boxTransform = boxView.get<components::Transform>(boxEntity);
            if (boxTransform.gridpos.x == newX &&
                boxTransform.gridpos.y == newY)
            {
                int boxNewX = newX + dx;
                int boxNewY = newY + dy;
                bool boxCanMove = true;

                for (auto otherBox : boxView)
                {
                    auto &otherBoxTransform = boxView.get<components::Transform>(otherBox);
                    if (otherBoxTransform.gridpos.x == boxNewX &&
                        otherBoxTransform.gridpos.y == boxNewY)
                    {
                        boxCanMove = false;
                    }
                }
                for (auto wallEntity : wallView)
                {
                    auto &wallTransform = wallView.get<components::Transform>(wallEntity);
                    if (wallTransform.gridpos.x == boxNewX &&
                        wallTransform.gridpos.y == boxNewY)
                    {
                        boxCanMove = false;
                    }
                }

                if (boxCanMove)
                {
                    boxTransform.position.x += dx * tileSize;
                    boxTransform.position.y -= dy * tileSize;
                    boxTransform.gridpos.x += dx;
                    boxTransform.gridpos.y += dy;

                    entt::entity soundEntity = registry.create();
                    auto &sound = registry.emplace<components::AudioSource>(soundEntity);
                    // 配置音效参数
                    sound.filePath = "artAssets/box.mp3";
                    sound.isMusic = false;   // 音效类型
                    sound.loop = false;      // 不循环
                    sound.shouldPlay = true; // 立即播放
                    sound.volume = 1.0f;
                    // registry.emplace<components::DestroyAfterPlay>(soundEntity);

                    auto wallView = registry.view<components::AudioSource>();
                    size_t i = 0;
                    for (auto wallEntity : wallView)
                    {
                        i++;
                    }
                    std::cout << i << "\n";
                }
                else
                {
                    canMove = false;
                }
            }
        }

        if (canMove)
        {
            transform.position.x += dx * tileSize;
            transform.position.y -= dy * tileSize;
            transform.gridpos.x = newX;
            transform.gridpos.y = newY;
        }
    }
}
#include <sokoFold_renderer.h>
#include <format>
#include <iostream>
void RenderSystem(entt::registry &registry, sokoFold_renderer::Renderer &renderer)
{
    renderer.BeginBatch();
    auto view = registry.view<components::Transform, components::Render>();

    for (auto entity : view)
    {
        auto &transform = view.get<components::Transform>(entity);
        auto &render = view.get<components::Render>(entity);

        // 计算物体的显示位置和大小
        glm::vec2 position = transform.position;
        glm::vec2 size = transform.size;
        float rad = transform.rad;

        // 绘制物体
        // std::cout << std::format("dfsdfsad_{}\n", render.textureName);
        renderer.DrawQuad(position, size, render.textureName, render.colorTint, rad);
    }
    // renderer.EndBatch();
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
            int x = transform.gridpos.x;
            int y = transform.gridpos.y;
            renderedMap[y][x] = '#';
        }

        // **画目标点（.）**
        auto goalView = registry.view<components::Transform, components::Goal>();
        for (auto goalEntity : goalView)
        {
            auto &transform = goalView.get<components::Transform>(goalEntity);
            int x = transform.gridpos.x;
            int y = transform.gridpos.y;
            renderedMap[y][x] = '.';
        }

        // **画箱子**
        auto boxView = registry.view<components::Transform, components::Box>();
        for (auto boxEntity : boxView)
        {
            auto &transform = boxView.get<components::Transform>(boxEntity);
            int x = transform.gridpos.x;
            int y = transform.gridpos.y;
            renderedMap[y][x] = '$';
        }

        // **画玩家**
        auto playerView = registry.view<components::Transform, components::Player>();
        for (auto playerEntity : playerView)
        {
            auto &transform = playerView.get<components::Transform>(playerEntity);
            int x = transform.gridpos.x;
            int y = transform.gridpos.y;
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
    auto groundView = registry.view<components::Ground>();
    for (auto entity : groundView)
    {
        registry.destroy(entity);
    }

    auto playerView = registry.view<components::Player>();
    for (auto entity : playerView)
    {
        registry.destroy(entity);
    }

    auto boxView = registry.view<components::Box>();
    for (auto entity : boxView)
    {
        registry.destroy(entity);
    }

    auto goalView = registry.view<components::Goal>();
    for (auto entity : goalView)
    {
        registry.destroy(entity);
    }

    auto wallView = registry.view<components::Wall>();
    for (auto entity : wallView)
    {
        registry.destroy(entity);
    }
    auto bgView = registry.view<components::Background>();
    for (auto entity : bgView)
    {
        registry.destroy(entity);
    }
    // 清空所有旧实体
    auto view = registry.view<components::Level>();
    for (auto entity : view)
    {
        registry.remove<components::Level>(entity); // 移除之前的关卡数据
    }

    auto levelEntity = registry.create();
    registry.emplace<components::Level>(levelEntity, levels[levelIndex]);

    // load background
    auto purewhite = registry.create();
    registry.emplace<components::Transform>(purewhite, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
    registry.emplace<components::Render>(purewhite, "background", glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    registry.emplace<components::Background>(purewhite);
    //^

    // 遍历地图并创建对应的实体
    float tmptilesize = fmin(2.0f / levels[levelIndex].size(), 2.0f / levels[levelIndex][0].size());
    tileSize = tmptilesize;
    for (int y = 0; y < levels[levelIndex].size(); ++y)
    {
        for (int x = 0; x < levels[levelIndex][y].size(); ++x)
        {
            char tile = levels[levelIndex][y][x];

            if (tile == '@')
            {
                // 玩家
                auto playerEntity = registry.create();
                registry.emplace<components::Transform>(playerEntity, glm::vec2((x + 0.5f) * tmptilesize - 1.0f, 1.0f - (y + 0.5f) * tmptilesize), glm::vec2(tmptilesize, tmptilesize), glm::ivec2(x, y));
                registry.emplace<components::Render>(playerEntity, "player");
                registry.emplace<components::Player>(playerEntity);
            }
        }
    }
    for (int y = 0; y < levels[levelIndex].size(); ++y)
    {
        for (int x = 0; x < levels[levelIndex][y].size(); ++x)
        {
            char tile = levels[levelIndex][y][x];
            if (tile == '.')
            {
                // 目标点
                auto goalEntity = registry.create();
                registry.emplace<components::Transform>(goalEntity, glm::vec2((x + 0.5f) * tmptilesize - 1.0f, 1.0f - (y + 0.5f) * tmptilesize), glm::vec2(tmptilesize, tmptilesize), glm::ivec2(x, y));
                registry.emplace<components::Render>(goalEntity, "goal");
                registry.emplace<components::Goal>(goalEntity);
            }
        }
    }
    for (int y = 0; y < levels[levelIndex].size(); ++y)
    {
        for (int x = 0; x < levels[levelIndex][y].size(); ++x)
        {
            char tile = levels[levelIndex][y][x];

            if (tile == '#')
            {
                // **墙壁**
                auto wallEntity = registry.create();
                registry.emplace<components::Transform>(wallEntity, glm::vec2((x + 0.5f) * tmptilesize - 1.0f, 1.0f - (y + 0.0f) * tmptilesize), glm::vec2(tmptilesize, tmptilesize * 2), glm::ivec2(x, y));
                registry.emplace<components::Render>(wallEntity, "wall");
                registry.emplace<components::Wall>(wallEntity); // ✅ **正确创建墙壁**
            }
            else if (tile == '$')
            {
                // 箱子
                auto boxEntity = registry.create();
                registry.emplace<components::Transform>(boxEntity, glm::vec2((x + 0.5f) * tmptilesize - 1.0f, 1.0f - (y + 0.5f) * tmptilesize), glm::vec2(tmptilesize, tmptilesize), glm::ivec2(x, y));
                registry.emplace<components::Render>(boxEntity, "crate");
                registry.emplace<components::Box>(boxEntity);
            }
        }
    }
    for (int y = 0; y < levels[levelIndex].size(); ++y)
    {
        for (int x = 0; x < levels[levelIndex][y].size(); ++x)
        {
            char tile = levels[levelIndex][y][x];

            // Ground
            auto playerEntity = registry.create();
            registry.emplace<components::Transform>(playerEntity, glm::vec2((x + 0.5f) * tmptilesize - 1.0f, 1.0f - (y + 0.5f) * tmptilesize), glm::vec2(tmptilesize, tmptilesize), glm::ivec2(x, y));
            registry.emplace<components::Render>(playerEntity, "ground");
            registry.emplace<components::Ground>(playerEntity);
        }
    }
    // load background
    auto backgroundEntity = registry.create();
    registry.emplace<components::Transform>(backgroundEntity, glm::vec2(0.0f, 0.0f), glm::vec2(SCR_WIDTH, SCR_HEIGHT));
    registry.emplace<components::Render>(backgroundEntity, "white");
    registry.emplace<components::Background>(backgroundEntity);
    //^
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
            int x = transform.gridpos.x;
            int y = transform.gridpos.y;
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
void HandleMainMenuInput(entt::registry &registry, GLFWwindow *window)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);
        if (g_KeyState[GLFW_KEY_ESCAPE])
        {
            glfwSetWindowShouldClose(window, true);
            g_KeyState[GLFW_KEY_ESCAPE] = false;
        }

        // **方向键选择菜单项**
        if (g_KeyState[GLFW_KEY_UP])
        {
            game.selectedMenuOption = (game.selectedMenuOption - 1 + 2) % 2;
            g_KeyState[GLFW_KEY_UP] = false; // 处理后清除按键状态
            entt::entity soundEntity = registry.create();
            auto &sound = registry.emplace<components::AudioSource>(soundEntity);
            // 配置音效参数
            sound.filePath = "artAssets/menu-sound-1-98856.mp3";
            sound.isMusic = false;   // 音效类型
            sound.loop = false;      // 不循环
            sound.shouldPlay = true; // 立即播放
            sound.volume = 1.0f;
            // registry.emplace<components::DestroyAfterPlay>(soundEntity);
        }
        if (g_KeyState[GLFW_KEY_DOWN])
        {
            game.selectedMenuOption = (game.selectedMenuOption + 1) % 2;
            g_KeyState[GLFW_KEY_DOWN] = false;
            entt::entity soundEntity = registry.create();
            auto &sound = registry.emplace<components::AudioSource>(soundEntity);
            // 配置音效参数
            sound.filePath = "artAssets/menu-sound-1-98856.mp3";
            sound.isMusic = false;   // 音效类型
            sound.loop = false;      // 不循环
            sound.shouldPlay = true; // 立即播放
            sound.volume = 1.0f;
            // registry.emplace<components::DestroyAfterPlay>(soundEntity);
        }

        // **按 Enter 选择菜单项**
        if (g_KeyState[GLFW_KEY_ENTER])
        {
            if (game.selectedMenuOption == 0) // Start Game
            {
                game.state = components::GameState::LevelSelect;
            }
            else if (game.selectedMenuOption == 1) // Exit
            {
                glfwSetWindowShouldClose(window, true);
            }
            g_KeyState[GLFW_KEY_ENTER] = false; // 处理后清除状态
        }
    }
}
void HandleLevelSelectInput(entt::registry &registry)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);
        if (g_KeyState[GLFW_KEY_ESCAPE])
        {
            game.state = components::GameState::MainMenu;
            g_KeyState[GLFW_KEY_ESCAPE] = false;
            return;
        }

        // 方向键选择关卡
        if (g_KeyState[GLFW_KEY_UP])
        {
            game.selectedLevel = (game.selectedLevel - 1 + levels.size()) % levels.size();
            g_KeyState[GLFW_KEY_UP] = false; // 处理后重置状态，避免重复触发
            entt::entity soundEntity = registry.create();
            auto &sound = registry.emplace<components::AudioSource>(soundEntity);
            // 配置音效参数
            sound.filePath = "artAssets/menu-sound-1-98856.mp3";
            sound.isMusic = false;   // 音效类型
            sound.loop = false;      // 不循环
            sound.shouldPlay = true; // 立即播放
            sound.volume = 1.0f;
            // registry.emplace<components::DestroyAfterPlay>(soundEntity);
        }
        if (g_KeyState[GLFW_KEY_DOWN])
        {
            game.selectedLevel = (game.selectedLevel + 1) % levels.size();
            g_KeyState[GLFW_KEY_DOWN] = false;
            entt::entity soundEntity = registry.create();
            auto &sound = registry.emplace<components::AudioSource>(soundEntity);
            // 配置音效参数
            sound.filePath = "artAssets/menu-sound-1-98856.mp3";
            sound.isMusic = false;   // 音效类型
            sound.loop = false;      // 不循环
            sound.shouldPlay = true; // 立即播放
            sound.volume = 1.5f;
            // registry.emplace<components::DestroyAfterPlay>(soundEntity);
        }

        // 按 Enter 进入游戏
        if (g_KeyState[GLFW_KEY_ENTER])
        {
            game.state = components::GameState::Playing;
            LoadLevelData(registry, game.selectedLevel);
            g_KeyState[GLFW_KEY_ENTER] = false;
        }
    }
}
void RenderMainMenu(entt::registry &registry, sokoFold_renderer::Renderer &renderer)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);

        renderer.BeginBatch();

        // 绘制背景
        renderer.DrawQuad(glm::vec2(+0.0f, +0.0f), glm::vec2(2.0f, 2.0f), "white", glm::vec4(1.0f));

        // 选中的选项用红色显示，否则用白色
        glm::vec4 startColor = (game.selectedMenuOption == 0) ? glm::vec4(1.0f, 0.5f, 0.5f, 1.0f) : glm::vec4(1.0f);
        glm::vec4 exitColor = (game.selectedMenuOption == 1) ? glm::vec4(0.5f, 1.0f, 0.5f, 1.0f) : glm::vec4(1.0f);

        // 绘制 "Start Game" 按钮
        renderer.DrawQuad(glm::vec2(0.0f, +1.0f / 6.0f), glm::vec2(1.0f, 1.0f / 6.0f), "button", startColor);

        // 绘制 "Exit" 按钮
        renderer.DrawQuad(glm::vec2(0.0f, -1.0f / 6.0f), glm::vec2(1.0f, 1.0f / 6.0f), "button", exitColor);

        renderer.EndBatch();
    }
    renderer.RenderText("SokoFold", 0.0f, +4.0f / 6.0f, 24.0f / 10000.0f, glm::vec3(0.5137, 0.0, 0.0));
    renderer.RenderText("Start", 0.0f, +1.0f / 6.0f, 12.0f / 10000.0f, glm::vec3(0.5, 0.8, 0.2));
    renderer.RenderText("Exit ", 0.0f, -1.0f / 6.0f, 12.0f / 10000.0f, glm::vec3(0.8, 0.2, 0.2));
}
void RenderLevelSelect(entt::registry &registry, sokoFold_renderer::Renderer &renderer)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);

        renderer.BeginBatch();

        // 绘制背景
        renderer.DrawQuad(glm::vec2(0.0f, 0.0f), glm::vec2(2.0f, 2.0f), "white", glm::vec4(1.0f));

        // 绘制关卡按钮
        for (int i = 0; i < levels.size(); i++)
        {
            glm::vec4 color = (i == game.selectedLevel) ? glm::vec4(1.0f, 0.5f, 0.5f, i * 0.5f + 0.1f) : glm::vec4(1.0f);
            renderer.DrawQuad(glm::vec2(0.0f, (1.0f - i) / 6.0f), glm::vec2(1.0f, 1.0f / 6.0f), "button", color, 0.0f);
        }

        renderer.EndBatch();
    }
    for (int i = 0; i < levels.size(); i++)
    {
        renderer.RenderText("Level " + std::to_string(i + 1), 0.0f, (1.0f - i) / 6.0f, 12.0f / 10000.0f, glm::vec4(0.1f, 0.2f, 0.9f, 0.1f));
    }
}
// void AudioCleanupSystem(entt::registry &registry)
// {
//     std::vector<entt::entity> toDestroy;

//     // 收集需要销毁的实体
//     auto view = registry.view<components::AudioSource>();
//     for (auto entity : view)
//     {
//         auto &audio = view.get<components::AudioSource>(entity);

//         // 非音乐且播放完毕的临时音效
//         if (!audio.isMusic && !audio.isPlaying)
//         {
//             // // 释放音频资源
//             // if (audio.handle.pDataSource != NULL)
//             // {
//             //     ma_sound_uninit(&audio.handle);
//             // }
//             toDestroy.push_back(entity);
//         }
//     }

//     // 统一销毁实体
//     for (auto entity : toDestroy)
//     {
//         registry.destroy(entity);
//     }
// }
#include <iostream>
#include <chrono>
#include <thread>
#include <sokoFold_renderer.h>
#include <sokoFold_audiosystem.h>
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
sokoFold_renderer::Renderer *RENDERER;
int main()
{
    entt::registry registry;
    int currentLevel = 0;

    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(800, 600, std::string("SokobanGame").c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    GLFWimage window_icon;
    window_icon.pixels = stbi_load("artAssets/inferno.png", &window_icon.width, &window_icon.height, 0, 4); // rgba channels
    glfwSetWindowIcon(window, 1, &window_icon);
    stbi_image_free(window_icon.pixels);

    glfwMakeContextCurrent(window);
    sokoFold_renderer::Renderer renderer(800, 600, window); // 创建 OpenGL 渲染器
    RENDERER = &renderer;
    renderer.LoadFont("artAssets/GOTHIC.TTF", 24 * 4);
    // 载入纹理
    renderer.loadTexture("wall", "artAssets/wall_001.png");
    renderer.loadTexture("crate", "artAssets/iceBox.png");
    renderer.loadTexture("goal", "artAssets/portal_001.png");
    renderer.loadTexture("player", "artAssets/inferno.png");
    renderer.loadTexture("background", "artAssets/bb3c7316dd9515f1f8de28c9b2016cd.jpg");
    renderer.loadTexture("button", "artAssets/iceBox.png");
    renderer.loadTexture("white", "artAssets/white.png");
    renderer.loadTexture("ground", "artAssets/tile_001.png");

    // 初始化 OpenGL
    GladGLContext *gl;
    gl = (GladGLContext *)calloc(1, sizeof(GladGLContext));
    if (!gl)
    {
        throw std::invalid_argument("Failed to create context");
    }
    int version = gladLoadGLContext(gl, glfwGetProcAddress);
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
    gl->Viewport(0, 0, 800, 600);
    gl->Enable(GL_BLEND);
    gl->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 初始化游戏状态
    auto gameEntity = registry.create();
    registry.emplace<components::Game>(gameEntity);

    // FPS 计时变量
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    AudioSystem audioSystem(registry);
    entt::entity bgm = registry.create();
    auto &bgmAudio = registry.emplace<components::AudioSource>(bgm);
    bgmAudio.filePath = "artAssets/GODDESS OF VICTORY NIKKE Global Theme Song TuNGSTeN Hiroyuki SAWANO feat. mizuki Full ver by Mori A._3.mp3";
    bgmAudio.isMusic = true;
    bgmAudio.loop = true;
    bgmAudio.shouldPlay = true; // 设置播放标记
    bgmAudio.volume = 0.1f;

    float t = 0;
    while (!glfwWindowShouldClose(window))
    {
        audioSystem.Update();
        // AudioCleanupSystem(registry);
        // **计算 deltaTime**
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
        t += deltaTime;
        lastTime = currentTime;

        // 计算 FPS
        static double lastFpsUpdateTime = glfwGetTime();
        static int fps = 0;
        frameCount++;
        if (glfwGetTime() - lastFpsUpdateTime >= 1.0) // 每秒更新一次
        {
            fps = frameCount;
            frameCount = 0;
            lastFpsUpdateTime = glfwGetTime();

            // **更新窗口标题**
            std::string newTitle = "SokobanGame - FPS: " + std::to_string(fps);
            glfwSetWindowTitle(window, newTitle.c_str());
        }

        // 处理输入
        glfwPollEvents();

        auto gameView = registry.view<components::Game>();
        for (auto entity : gameView)
        {
            auto &game = gameView.get<components::Game>(entity);

            if (game.state == components::GameState::MainMenu)
            {
                HandleMainMenuInput(registry, window);
                // **清除 OpenGL 画面**
                gl->ClearColor(0.3059f, 0.2627f, 0.0667f, 1.0f);
                gl->Clear(GL_COLOR_BUFFER_BIT);
                RenderMainMenu(registry, renderer);
                // std::cout << std::format("MainMenu\n");
            }
            else if (game.state == components::GameState::LevelSelect)
            {
                HandleLevelSelectInput(registry);
                // **清除 OpenGL 画面**
                gl->ClearColor(0.3059f, 0.2627f, 0.0667f, 1.0f);
                gl->Clear(GL_COLOR_BUFFER_BIT);
                RenderLevelSelect(registry, renderer);
                // std::cout << std::format("LevelSelect\n");
            }
            else if (game.state == components::GameState::Playing)
            {
                // 游戏逻辑
                LevelAnimationSystem(registry, t, deltaTime);
                MovementSystem(registry);
                // **清除 OpenGL 画面**
                gl->ClearColor(0.3059f, 0.2627f, 0.0667f, 1.0f);
                gl->Clear(GL_COLOR_BUFFER_BIT);
                RenderSystem(registry, renderer);
                // std::cout << std::format("Playing\n");
                // **终端渲染**
                // TerminalRenderSystem(registry);

                // 过关检测
                if (CheckLevelComplete(registry))
                {
                    std::cout << "🎉 Level " << game.selectedLevel + 1 << " finish!" << std::endl;
                    game.state = components::GameState::LevelSelect; // 返回选关界面
                }
            }
        }
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glfwMakeContextCurrent(window);
    GladGLContext *gl;
    gl = (GladGLContext *)calloc(1, sizeof(GladGLContext));
    if (!gl)
    {
        throw std::invalid_argument("Failed to create context");
    }
    int version = gladLoadGLContext(gl, glfwGetProcAddress);
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    gl->Viewport(0, 0, width, height);
    RENDERER->SetProjection(width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        g_KeyState[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        g_KeyState[key] = false;
    }
}