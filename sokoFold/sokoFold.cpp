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

        // 绘制物体
        // std::cout << std::format("dfsdfsad_{}\n", render.textureName);
        renderer.DrawQuad(position, size, render.textureName, render.colorTint);
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
    registry.emplace<components::Transform>(purewhite, glm::vec2(400.0f, 300.0f), glm::vec2(400.0f, 300.0f));
    registry.emplace<components::Render>(purewhite, "white", glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
    registry.emplace<components::Background>(purewhite);
    //^

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
                registry.emplace<components::Transform>(playerEntity, glm::vec2(x * 50.0f + 25.0f, y * 50.0f + 25.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(playerEntity, "player");
                registry.emplace<components::Player>(playerEntity);
            }
            else if (tile == '#')
            {
                // **墙壁**
                auto wallEntity = registry.create();
                registry.emplace<components::Transform>(wallEntity, glm::vec2(x * 50.0f + 25.0f, y * 50.0f + 25.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(wallEntity, "wall");
                registry.emplace<components::Wall>(wallEntity); // ✅ **正确创建墙壁**
            }
            else if (tile == '$')
            {
                // 箱子
                auto boxEntity = registry.create();
                registry.emplace<components::Transform>(boxEntity, glm::vec2(x * 50.0f + 25.0f, y * 50.0f + 25.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(boxEntity, "crate");
                registry.emplace<components::Box>(boxEntity);
            }
            else if (tile == '.')
            {
                // 目标点
                auto goalEntity = registry.create();
                registry.emplace<components::Transform>(goalEntity, glm::vec2(x * 50.0f + 25.0f, y * 50.0f + 25.0f), glm::vec2(50.0f, 50.0f));
                registry.emplace<components::Render>(goalEntity, "goal");
                registry.emplace<components::Goal>(goalEntity);
            }
        }
    }
    // load background
    auto backgroundEntity = registry.create();
    registry.emplace<components::Transform>(backgroundEntity, glm::vec2(400.0f, 300.0f), glm::vec2(800.0f, 600.0f));
    registry.emplace<components::Render>(backgroundEntity, "background");
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
void HandleMenuInput(entt::registry &registry, GLFWwindow *window)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);

        static bool lastUp = false, lastDown = false, lastEnter = false;

        bool currentUp = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        bool currentDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        bool currentEnter = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;

        if (currentUp && !lastUp)
        {
            game.selectedLevel = (game.selectedLevel - 1 + levels.size()) % levels.size();
        }
        if (currentDown && !lastDown)
        {
            game.selectedLevel = (game.selectedLevel + 1) % levels.size();
        }
        if (currentEnter && !lastEnter)
        {
            game.state = components::GameState::Playing;
            LoadLevelData(registry, game.selectedLevel);
        }

        lastUp = currentUp;
        lastDown = currentDown;
        lastEnter = currentEnter;
    }
}
void RenderMenu(entt::registry &registry, sokoFold_renderer::Renderer &renderer)
{
    auto gameView = registry.view<components::Game>();
    for (auto entity : gameView)
    {
        auto &game = gameView.get<components::Game>(entity);

        renderer.BeginBatch();

        // 绘制背景
        renderer.DrawQuad(glm::vec2(400, 300), glm::vec2(800, 600), "background", glm::vec4(1.0f));

        // 绘制关卡按钮
        for (int i = 0; i < levels.size(); i++)
        {
            glm::vec4 color = (i == game.selectedLevel) ? glm::vec4(1.0f, 0.5f, 0.5f, 1.0f) : glm::vec4(1.0f);
            renderer.DrawQuad(glm::vec2(400, 200 + i * 50), glm::vec2(200, 40), "button", color);
        }

        renderer.EndBatch();
    }
}
#include <iostream>
#include <chrono>
#include <thread>
#include <sokoFold_renderer.h>
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
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
    glfwMakeContextCurrent(window);
    sokoFold_renderer::Renderer renderer(800, 600, window); // 创建 OpenGL 渲染器

    // 载入纹理
    renderer.loadTexture("wall", "artAssets/2647570.png");
    renderer.loadTexture("crate", "artAssets/icon1.png");
    renderer.loadTexture("goal", "artAssets/icon2.png");
    renderer.loadTexture("player", "artAssets/246139_8_sq.png");
    renderer.loadTexture("background", "artAssets/bb3c7316dd9515f1f8de28c9b2016cd.jpg");
    renderer.loadTexture("button", "artAssets/icon1.png");
    renderer.loadTexture("white", "artAssets/white.png");

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

    while (!glfwWindowShouldClose(window))
    {
        // **计算 deltaTime**
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
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

            if (game.state == components::GameState::Menu)
            {
                // 处理选关界面
                HandleMenuInput(registry, window);
                RenderMenu(registry, renderer);
            }
            else if (game.state == components::GameState::Playing)
            {
                // 游戏逻辑
                MovementSystem(registry, window);
                // **清除 OpenGL 画面**
                gl->ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                gl->Clear(GL_COLOR_BUFFER_BIT);
                RenderSystem(registry, renderer);
                // **终端渲染**
                // TerminalRenderSystem(registry);

                // 过关检测
                if (CheckLevelComplete(registry))
                {
                    std::cout << "🎉 Level " << game.selectedLevel + 1 << " finish!" << std::endl;
                    game.state = components::GameState::Menu; // 返回选关界面
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
}
