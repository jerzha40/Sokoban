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
    void LoadLevel(int levelIndex); // 修改为可指定关卡
    int GetCurrentLevel() const { return m_CurrentLevel; }

private:
    std::vector<std::vector<std::string>> m_Levels; // 所有关卡数据
    int m_CurrentLevel = 0;                         // 当前关卡索引

    Renderer *m_Renderer; // 添加成员变量
    int m_Width, m_Height;
    GLFWwindow *window;
    struct
    {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
    } m_LastKeys; // 记录上一帧按键状态
    std::vector<std::string> m_Level;
    int m_PlayerX, m_PlayerY;

    void MovePlayer(int dx, int dy);
    bool IsLevelComplete();
};

#endif
