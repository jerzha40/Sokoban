#include "game.h"
#include <iostream>

Game::Game(int width, int height, GLFWwindow *window) : m_Width(width), m_Height(height), window(window)
{
    m_Renderer = new Renderer(width, height, window);
    LoadLevel();
}

Game::~Game()
{
    delete m_Renderer;
}

void Game::LoadLevel()
{
    m_Level = {
        "#######",
        "#     #",
        "#  $  #",
        "#  @  #",
        "#  .  #",
        "#     #",
        "#######"};

    for (int y = 0; y < m_Level.size(); ++y)
    {
        for (int x = 0; x < m_Level[y].size(); ++x)
        {
            if (m_Level[y][x] == '@')
            {
                m_PlayerX = x;
                m_PlayerY = y;
            }
        }
    }
}

void Game::Update()
{
    // 这里可以处理玩家输入
    // 获取当前按键状态
    bool currentUp = glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
    bool currentDown = glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
    bool currentLeft = glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
    bool currentRight = glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

    // 处理上方向键（仅在按键刚按下时触发）
    if (currentUp && !m_LastKeys.up)
    {
        MovePlayer(0, -1);
    }
    m_LastKeys.up = currentUp;

    // 处理下方向键
    if (currentDown && !m_LastKeys.down)
    {
        MovePlayer(0, 1);
    }
    m_LastKeys.down = currentDown;

    // 处理左方向键
    if (currentLeft && !m_LastKeys.left)
    {
        MovePlayer(-1, 0);
    }
    m_LastKeys.left = currentLeft;

    // 处理右方向键
    if (currentRight && !m_LastKeys.right)
    {
        MovePlayer(1, 0);
    }
    m_LastKeys.right = currentRight;

    // 检查关卡是否完成
    if (IsLevelComplete())
    {
        std::cout << "Level Clear!" << std::endl;
        // 可以在此处重置关卡或加载下一关
    }
}

void Game::Render()
{
    m_Renderer->BeginBatch();

    const float tileSize = 50.0f;
    for (int y = 0; y < m_Level.size(); ++y)
    {
        for (int x = 0; x < m_Level[y].size(); ++x)
        {
            glm::vec2 pos(x * tileSize + tileSize / 2.0f, y * tileSize + tileSize / 2.0f);
            glm::vec2 size(tileSize, tileSize);

            switch (m_Level[y][x])
            {
            case '#': // 墙
                m_Renderer->DrawQuad(pos, size, glm::vec3(0.5f, 0.5f, 0.5f));
                break;
            case '@': // 玩家
                m_Renderer->DrawQuad(pos, size, glm::vec3(0.0f, 0.0f, 1.0f));
                break;
            case '$': // 箱子
                m_Renderer->DrawQuad(pos, size, glm::vec3(0.6f, 0.3f, 0.2f));
                break;
            case '.': // 目标点
                m_Renderer->DrawQuad(pos, size, glm::vec3(1.0f, 0.0f, 0.0f));
                break;
            }
        }
    }

    m_Renderer->EndBatch(); // 统一提交所有绘制命令
}

void Game::MovePlayer(int dx, int dy)
{
    int newX = m_PlayerX + dx;
    int newY = m_PlayerY + dy;

    // 边界检查
    if (newX < 0 || newY < 0 || newY >= m_Level.size() || newX >= m_Level[newY].size())
    {
        return;
    }

    char targetTile = m_Level[newY][newX];

    // 移动到空地或目标点
    if (targetTile == ' ' || targetTile == '.')
    {
        m_Level[m_PlayerY][m_PlayerX] = ' ';
        m_PlayerX = newX;
        m_PlayerY = newY;
        m_Level[m_PlayerY][m_PlayerX] = '@';
    }
    // 推动箱子
    else if (targetTile == '$')
    {
        int boxNewX = newX + dx;
        int boxNewY = newY + dy;

        // 检查箱子目标位置是否合法
        if (boxNewX < 0 || boxNewY < 0 || boxNewY >= m_Level.size() || boxNewX >= m_Level[boxNewY].size())
        {
            return;
        }

        char boxTargetTile = m_Level[boxNewY][boxNewX];
        if (boxTargetTile == ' ' || boxTargetTile == '.')
        {
            // 移动玩家
            m_Level[m_PlayerY][m_PlayerX] = ' ';
            m_PlayerX = newX;
            m_PlayerY = newY;
            m_Level[m_PlayerY][m_PlayerX] = '@';

            // 移动箱子
            m_Level[boxNewY][boxNewX] = '$';
            // m_Level[newY][newX] = ' ';
        }
    }
}
bool Game::IsLevelComplete()
{
    for (const auto &row : m_Level)
    {
        if (row.find('.') != std::string::npos)
            return false;
    }
    return true;
}
