#pragma once
#include <string>
#include <glad/gl.h>

class Shader
{
public:
    GladGLContext *gl;
    Shader(const char *vertexPath, const char *fragmentPath, GladGLContext *gl);
    ~Shader();
    void use() const;

private:
    unsigned int ID;
    void checkCompileErrors(unsigned int shader, const std::string &type);
};