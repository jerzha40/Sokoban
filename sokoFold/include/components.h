#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <miniaudio.h>
namespace components
{
    // struct DestroyAfterPlay
    // {
    //     float delay = 0.5f; // 安全销毁延迟
    // };
    struct AudioSource
    {
        std::string filePath;
        bool isMusic = false;    // 是否背景音乐
        bool loop = false;       // 是否循环
        bool shouldPlay = false; // 需要播放标记
        bool isPlaying = false;  // 当前播放状态
        float volume = 1.0f;
        ma_sound handle; // miniaudio对象
        size_t pendingDestroy = 0;
    };
    enum class GameState
    {
        MainMenu,    // 主菜单
        LevelSelect, // 选关界面
        Playing      // 游戏中
    };
    struct Game
    {
        GameState state = GameState::MainMenu; // 初始进入主菜单
        int selectedLevel = 0;                 // 选中的关卡
        int selectedMenuOption = 0;            // 选中的主菜单选项 (0 = Start Game, 1 = Exit)
    };
    struct Transform
    {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec2 velocity;
    };
    struct Render
    {
        std::string textureName;
        glm::vec4 colorTint = glm::vec4(1.0f); // 可选的颜色调色
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
    struct Background
    {
        /* data */
    };
} // namespace components
#endif
