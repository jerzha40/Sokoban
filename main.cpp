#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <glad/gl.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "game.h"
#include <iostream>

// 屏幕尺寸
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char **argv)
{
    // 初始化 SDL 视频子系统
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 设置 OpenGL 属性（这里设置为 4.3 Core Profile）
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // 创建 SDL 窗口，使用 SDL_WINDOW_OPENGL 标志
    SDL_Window *window = SDL_CreateWindow("推箱子游戏",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCR_WIDTH, SCR_HEIGHT,
                                          SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 创建 OpenGL 上下文
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context)
    {
        std::cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 用 SDL_GL_GetProcAddress 加载 OpenGL 函数（使用 glad）
    int version = gladLoadGL((GLADloadproc)SDL_GL_GetProcAddress);
    std::cout << "Loaded OpenGL "
              << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << std::endl;

    // 初始化 ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    // 使用 SDL 后端初始化 ImGui（注意这里使用的是 SDL 后端，而非 GLFW）
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 创建游戏对象，Game 的构造函数应支持 SDL_Window* 作为参数
    Game game(SCR_WIDTH, SCR_HEIGHT, window);

    bool running = true;
    SDL_Event event;
    while (running)
    {
        // 处理所有 SDL 事件，同时传递给 ImGui 后端处理
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            // 当收到退出事件时退出循环
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // 清除背景颜色（示例使用 OpenGL 调用）
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 开始 ImGui 帧
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 更新和渲染游戏（Game 类内部实现你的推箱子逻辑和渲染）
        game.Update();
        game.Render();

        // 渲染 ImGui 界面
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // 交换窗口缓冲区
        SDL_GL_SwapWindow(window);
    }

    // 清理 ImGui 资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // 清理 SDL 资源
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
