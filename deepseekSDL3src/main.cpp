#include <glad/gles2.h>
#include "shader.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <iostream>

int main()
{
    // 初始化 SDL3
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }
    // 在创建窗口前设置 OpenGL 属性
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    // 创建窗口和 OpenGL 上下文
    SDL_Window *window = SDL_CreateWindow("OpenGL Triangle", 800, 600, SDL_WINDOW_OPENGL);
    if (!window)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    SDL_GL_MakeCurrent(window, context);
    // 初始化 GLAD
    if (!gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
    }
    const GLubyte *version = glGetString(GL_VERSION);
    if (version)
    {
        std::cout << "GLES Version: " << version << std::endl;
    }
    else
    {
        std::cerr << "Failed to get GLES version" << std::endl;
    }
    glViewport(0, 0, 800, 600);

    // 定义顶点数据（位置 + 颜色）
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下 (红)
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // 右下 (绿)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部 (蓝)
    };

    // 创建 VAO 和 VBO
    unsigned int VAO, VBO;
    glGenVertexArraysOES(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArrayOES(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 加载着色器
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // 主循环
    bool quit = false;
    SDL_Event event;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArrayOES(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    // 清理资源
    glDeleteVertexArraysOES(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}