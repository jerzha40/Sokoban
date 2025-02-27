#include <core/gmWindow.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace gmWindow
{

    GLFWwindow *gmWindow::getWindow()
    {
        return window;
    }
    GladGLContext *gmWindow::getGl()
    {
        return gl;
    }
    bool gmWindow::isWindowClosed()
    {
        return glfwWindowShouldClose(window);
    }
    void gmWindow::run()
    {
        glfwMakeContextCurrent(window);
        // input
        // -----
        glfwPollEvents();
        // render
        // ------
        gl->ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        gl->Clear(GL_COLOR_BUFFER_BIT);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }
    void gmWindow::set_framebuffer_size_callback(GLFWframebuffersizefun callback)
    {
        this->framebuffer_size_callback = callback;
        glfwSetFramebufferSizeCallback(window, this->framebuffer_size_callback);
    }
    void gmWindow::set_key_callback(GLFWkeyfun callback)
    {
        key_callback = callback;
        glfwSetKeyCallback(window, key_callback);
    }
    void gmWindow::set_window_icon(const char *filename)
    {
        window_icon.pixels = stbi_load(filename, &window_icon.width, &window_icon.height, 0, 4); // rgba channels
        glfwSetWindowIcon(window, 1, &window_icon);
        stbi_image_free(window_icon.pixels);
    }
    gmWindow::gmWindow(const gmWindowSpecification &spec) : mSpecification(spec)
    {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // glfw window creation
        // --------------------
        window = glfwCreateWindow(mSpecification.SCR_WIDTH, mSpecification.SCR_HEIGHT, mSpecification.WINDOW_NAME, NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            throw std::invalid_argument("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);
        gl = (GladGLContext *)calloc(1, sizeof(GladGLContext));
        if (!gl)
        {
            throw std::invalid_argument("Failed to create context");
        }

        int version = gladLoadGLContext(gl, glfwGetProcAddress);
        std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        // if ()
        // {
        //     std::cout << "Failed to initialize GLAD" << std::endl;
        //     throw std::invalid_argument("Failed to initialize GLAD");
        // }
        framebuffer_size_callback = default_framebuffer_size_callback;
        key_callback = processInput;
        glfwSetFramebufferSizeCallback(window, this->framebuffer_size_callback);
        glfwSetKeyCallback(window, key_callback);
    }

    gmWindow::~gmWindow()
    {
        free(gl);
    }
}
