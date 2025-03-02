#ifndef SOKOFOLD_TEXTURE_H
#define SOKOFOLD_TEXTURE_H
#include <glad/gl.h>
#include <string>
#include <GLFW/glfw3.h>
namespace sokoFold_texture
{
    class Texture
    {
    public:
        GladGLContext *gl;
        Texture(const std::string &path, GLFWwindow *);
        ~Texture();

        void bind(GLuint slot = 0) const;
        GLuint getID() const { return m_ID; }

    private:
        GLuint m_ID;
        int m_Width, m_Height, m_Channels;
    };
} // namespace sokoFold_texture
#endif
