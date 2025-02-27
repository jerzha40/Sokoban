#ifndef RENDERER_H
#define RENDERER_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <texture.h>
#include <unordered_map>

class Renderer
{
public:
    void loadTexture(const std::string &name, const std::string &path);
    Texture *getTexture(const std::string &name);
    Renderer(int windowWidth, int windowHeight, GLFWwindow *window);
    ~Renderer();

    void BeginBatch();
    void EndBatch();
    void DrawQuad(const glm::vec2 &position,
                  const glm::vec2 &size,
                  const std::string &textureName,
                  const glm::vec3 &colorTint = glm::vec3(1.0f));
    void SetProjection(int width, int height);

private:
    std::unordered_map<std::string, Texture *> m_Textures; // 纹理库
    void InitShader();
    void InitVertexData();

    GLuint m_ShaderProgram;
    GLuint m_VAO, m_VBO;
    int m_WindowWidth, m_WindowHeight;
    GLFWwindow *window;
    GladGLContext *gl;
};

#endif
