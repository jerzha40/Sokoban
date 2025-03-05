#include <glad/gl.h>
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); // 与 GLAD 生成的版本一致
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
    GladGLContext *gl;
    gl = (GladGLContext *)calloc(1, sizeof(GladGLContext));
    if (!gl)
    {
        throw std::invalid_argument("Failed to create context");
        std::cerr << "Failed to initialize GLAD" << std::endl;
        // SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    int version = gladLoadGLContext(gl, SDL_GL_GetProcAddress);
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    gl->Viewport(0, 0, 800, 600);

    // 定义顶点数据（位置 + 颜色）
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下 (红)
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // 右下 (绿)
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // 顶部 (蓝)
    };

    // 创建 VAO 和 VBO
    unsigned int VAO, VBO;
    gl->GenVertexArrays(1, &VAO);
    gl->GenBuffers(1, &VBO);
    gl->BindVertexArray(VAO);
    gl->BindBuffer(GL_ARRAY_BUFFER, VBO);
    gl->BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    gl->VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    gl->EnableVertexAttribArray(0);
    gl->VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    gl->EnableVertexAttribArray(1);

    // 加载着色器
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl", gl);

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

        gl->ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        gl->Clear(GL_COLOR_BUFFER_BIT);

        shader.use();
        gl->BindVertexArray(VAO);
        gl->DrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    // 清理资源
    gl->DeleteVertexArrays(1, &VAO);
    gl->DeleteBuffers(1, &VBO);
    // SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}