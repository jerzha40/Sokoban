#ifndef GAME_H
#define GAME_H
#include <renderer.h> // 添加这行
#include <vector>
#include <string>

class Game
{
public:
    Game(int width, int height, GLFWwindow *window);
    ~Game();

    void Update();
    void Render();

private:
    Renderer *m_Renderer; // 添加成员变量
    int m_Width, m_Height;
    GLFWwindow *window;
    std::vector<std::string> m_Level;
    int m_PlayerX, m_PlayerY;

    void LoadLevel();
    void MovePlayer(int dx, int dy);
    bool IsLevelComplete();
};

#endif
