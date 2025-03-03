#ifndef RENDERER_H
#define RENDERER_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer
{
public:
    Renderer(int windowWidth, int windowHeight, GLFWwindow *window);
    ~Renderer();

    void BeginBatch();
    void EndBatch();
    void DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec3 &color);
    void SetProjection(int width, int height);

private:
    void InitShader();
    void InitVertexData();

    GLuint m_ShaderProgram;
    GLuint m_VAO, m_VBO;
    int m_WindowWidth, m_WindowHeight;
    GLFWwindow *window;
    GladGLContext *gl;
};

#endif
