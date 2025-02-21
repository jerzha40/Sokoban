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
        "#  @  #",
        "#  $  #",
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

    if (m_Level[newY][newX] == ' ' || m_Level[newY][newX] == '.')
    {
        m_Level[m_PlayerY][m_PlayerX] = ' ';
        m_PlayerX = newX;
        m_PlayerY = newY;
        m_Level[m_PlayerY][m_PlayerX] = '@';
    }
    else if (m_Level[newY][newX] == '$')
    {
        int newBoxX = newX + dx;
        int newBoxY = newY + dy;

        if (m_Level[newBoxY][newBoxX] == ' ' || m_Level[newBoxY][newBoxX] == '.')
        {
            m_Level[m_PlayerY][m_PlayerX] = ' ';
            m_PlayerX = newX;
            m_PlayerY = newY;
            m_Level[m_PlayerY][m_PlayerX] = '@';
            m_Level[newBoxY][newBoxX] = '$';
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
