#ifndef GMWINDOW_H
#define GMWINDOW_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
// Function prototypes
// GLFWwindow *create_window(const char *name, int major, int minor);
// GladGLContext *create_context(GLFWwindow *window);
// void free_context(GladGLContext *context);
// void draw(GLFWwindow *window, GladGLContext *context, float r, float g, float b);
// void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

namespace gmWindow
{
    void default_framebuffer_size_callback(GLFWwindow *window, int width, int height);
    void processInput(GLFWwindow *window, int key, int scancode, int action, int mods);

    struct gmWindowSpecification
    {
        /* data */
        const unsigned int SCR_WIDTH = 800;
        const unsigned int SCR_HEIGHT = 600;
        const unsigned int GL_major = 3;
        const unsigned int GL_minor = 3;
        const char *WINDOW_NAME = "LearnOpenGL";
        gmWindowSpecification() {}
        gmWindowSpecification(const unsigned int WIDTH, const unsigned int HEIGHT, const char *NAME) : SCR_WIDTH(WIDTH), SCR_HEIGHT(HEIGHT), WINDOW_NAME(NAME) {}
    };

    class gmWindow
    {
    public:
    private:
        /* data */
        gmWindowSpecification mSpecification;
        GLFWwindow *window;
        GladGLContext *gl;
        GLFWframebuffersizefun framebuffer_size_callback;
        GLFWkeyfun key_callback;
        GLFWframebuffersizefun input_callback;
        GLFWimage window_icon;

    public:
        /* methods */
        gmWindow(const gmWindowSpecification &spec = gmWindowSpecification()); // construct use spec
        ~gmWindow();                                                           // deconstruct
        GLFWwindow *getWindow();                                               // return GLFWwindow
        GladGLContext *getGl();                                                // return GladGLContext
        bool isWindowClosed();                                                 //
        void run();                                                            // run engine
        void set_framebuffer_size_callback(GLFWframebuffersizefun callback);
        void set_key_callback(GLFWkeyfun callback);
        void set_window_icon(const char *);
    };

} // namespace gmWindow

#endif
