#include <SDL3/SDL.h>
#include <iostream>

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 创建两个窗口
    SDL_Window *window1 = SDL_CreateWindow("Red Window", 640, 480, SDL_WINDOW_OPENGL);
    if (!window1)
    {
        std::cerr << "SDL_CreateWindow Error (window1): " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window *window2 = SDL_CreateWindow("Yellow Window", 640, 480, SDL_WINDOW_OPENGL);
    if (!window2)
    {
        std::cerr << "SDL_CreateWindow Error (window2): " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window1);
        SDL_Quit();
        return 1;
    }

    // 为每个窗口创建独立的渲染器
    SDL_Renderer *renderer1 = SDL_CreateRenderer(window1, nullptr);
    if (!renderer1)
    {
        std::cerr << "SDL_CreateRenderer Error (renderer1): " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window1);
        SDL_DestroyWindow(window2);
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer2 = SDL_CreateRenderer(window2, nullptr);
    if (!renderer2)
    {
        std::cerr << "SDL_CreateRenderer Error (renderer2): " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer1);
        SDL_DestroyWindow(window1);
        SDL_DestroyWindow(window2);
        SDL_Quit();
        return 1;
    }

    bool window1_open = true;
    bool window2_open = true;
    bool running = true;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            // 如果收到 SDL_EVENT_QUIT 或窗口关闭事件，则退出程序
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                // 获取关闭窗口的ID
                Uint32 winID = event.window.windowID;
                // 判断是哪个窗口
                if (window1 && SDL_GetWindowID(window1) == winID)
                {
                    SDL_DestroyRenderer(renderer1);
                    SDL_DestroyWindow(window1);
                    window1 = nullptr;
                    window1_open = false;
                    SDL_Log("Red Window closed.");
                }
                else if (window2 && SDL_GetWindowID(window2) == winID)
                {
                    SDL_DestroyRenderer(renderer2);
                    SDL_DestroyWindow(window2);
                    window2 = nullptr;
                    window2_open = false;
                    SDL_Log("Yellow Window closed.");
                }
            }
        }

        // 绘制第一个窗口为红色背景
        SDL_SetRenderDrawColor(renderer1, 255, 0, 0, 255);
        SDL_RenderClear(renderer1);
        SDL_RenderPresent(renderer1);

        // 绘制第二个窗口为黄色背景（红+绿）
        SDL_SetRenderDrawColor(renderer2, 255, 255, 0, 255);
        SDL_RenderClear(renderer2);
        SDL_RenderPresent(renderer2);

        SDL_Delay(16); // 延时约16ms
    }

    SDL_DestroyRenderer(renderer1);
    SDL_DestroyRenderer(renderer2);
    SDL_DestroyWindow(window1);
    SDL_DestroyWindow(window2);
    SDL_Quit();
    return 0;
}
