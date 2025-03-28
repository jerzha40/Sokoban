#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char *vertexPath, const char *fragmentPath, GladGLContext *gl)
{
    this->gl = gl;

    // 读取文件（添加错误检查）
    std::ifstream vShaderFile, fShaderFile;
    std::string vertexCode, fragmentCode;
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        vShaderFile.close();
        fShaderFile.close();
    }
    catch (std::ifstream::failure &e)
    {
        std::cerr << "ERROR: Shader files not read: " << e.what() << std::endl;
    }

    // 编译着色器
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex = gl->CreateShader(GL_VERTEX_SHADER);
    gl->ShaderSource(vertex, 1, &vShaderCode, nullptr);
    gl->CompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = gl->CreateShader(GL_FRAGMENT_SHADER);
    gl->ShaderSource(fragment, 1, &fShaderCode, nullptr);
    gl->CompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 链接着色器程序
    ID = gl->CreateProgram();
    gl->AttachShader(ID, vertex);
    gl->AttachShader(ID, fragment);
    gl->LinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // 清理
    gl->DeleteShader(vertex);
    gl->DeleteShader(fragment);
}
void Shader::use() const
{
    gl->UseProgram(ID);
}

void Shader::checkCompileErrors(unsigned int shader, const std::string &type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        gl->GetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            gl->GetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR: " << type << "\n"
                      << infoLog << std::endl;
        }
    }
    else
    {
        gl->GetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            gl->GetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR\n"
                      << infoLog << std::endl;
        }
    }
}

Shader::~Shader()
{
    gl->DeleteProgram(ID);
}