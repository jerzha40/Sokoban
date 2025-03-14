// texture.cpp
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <sokoFold_texture.h>
#include <iostream>
#include <fstream>
namespace sokoFold_texture
{
    Texture::Texture(const std::string &path, GLFWwindow *window)
    {
        glfwMakeContextCurrent(window);
        gl = (GladGLContext *)calloc(1, sizeof(GladGLContext));
        int version = gladLoadGLContext(gl, glfwGetProcAddress);

        stbi_set_flip_vertically_on_load(true);

        std::ifstream file(path);
        if (!file.good())
        {
            std::cerr << "! cant find tex: " << path << std::endl;
        }
        else
        {
            std::cout << "\\/ tex exist: " << path << std::endl;
        }

        // 强制以 4 通道读取，避免丢失 Alpha
        unsigned char *data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 4);
        if (!data)
        {
            std::cout << "* cant: " << path << std::endl;
        }
        else
        {
            std::cout << "\\/ can: " << path << " (" << m_Width << "x" << m_Height << ")" << std::endl;
        }
        m_Channels = 4; // 统一设为 4，确保后续逻辑正确
        std::cout << "Channels: " << m_Channels << std::endl;

        gl->GenTextures(1, &m_ID);
        gl->BindTexture(GL_TEXTURE_2D, m_ID);

        // 设置纹理参数
        gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        GLenum format = (m_Channels == 4) ? GL_RGBA : GL_RGB;
        gl->TexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
        gl->GenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        // delete gl;
    }

    Texture::~Texture()
    {
        gl->DeleteTextures(1, &m_ID);
        delete gl;
    }

    void Texture::bind(GLuint slot) const
    {
        gl->ActiveTexture(GL_TEXTURE0 + slot);
        gl->BindTexture(GL_TEXTURE_2D, m_ID);
    }
} // namespace sokoFold_texture
