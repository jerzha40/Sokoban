#ifndef SOKOFOLD_RENDERER_H
#define SOKOFOLD_RENDERER_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sokoFold_texture.h>
#include <unordered_map>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace sokoFold_renderer
{
    class Renderer
    {
    public:
        void loadTexture(const std::string &name, const std::string &path);
        sokoFold_texture::Texture *getTexture(const std::string &name);
        Renderer(int windowWidth, int windowHeight, GLFWwindow *window);
        ~Renderer();

        void BeginBatch();
        void EndBatch();
        void DrawQuad(const glm::vec2 &position,
                      const glm::vec2 &size,
                      const std::string &textureName,
                      const glm::vec4 &colorTint = glm::vec4(1.0f),
                      const float rad = 0.0f);
        void SetProjection(int width, int height);
        void LoadFont(const std::string &fontPath, unsigned int fontSize);
        void RenderText(const std::string &text, float x, float y, float scale, glm::vec3 color);

    private:
        std::unordered_map<std::string, sokoFold_texture::Texture *> m_Textures; // 纹理库
        void InitShader();
        void InitVertexData();
        void InitTextRendering();
        GLuint m_ShaderProgram;
        GLuint m_VAO, m_VBO;
        int m_WindowWidth, m_WindowHeight;
        GLFWwindow *window;
        GladGLContext *gl;

        struct Character
        {
            GLuint TextureID;     // 字形纹理ID
            glm::ivec2 Size;      // 字形尺寸
            glm::ivec2 Bearing;   // 从基准线到字形左部/顶部的偏移
            unsigned int Advance; // 到下一个字形的水平距离
        };

        std::map<GLchar, Character> m_Characters;
        GLuint m_TextVAO, m_TextVBO;
        GLuint m_TextShader;
    };
} // namespace sokoFold_renderer

#endif
