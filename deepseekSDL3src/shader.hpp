#pragma once
#include <string>
#include <glad/gles2.h>

class Shader
{
public:
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();
    void use() const;

private:
    unsigned int ID;
    void checkCompileErrors(unsigned int shader, const std::string &type);
};