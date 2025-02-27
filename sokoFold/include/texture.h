#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/gl.h>
#include <string>
#include <GLFW/glfw3.h>

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

#endif
