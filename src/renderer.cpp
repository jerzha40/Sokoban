#include <renderer.h>
#include <iostream>

const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;

uniform mat4 projection;
uniform mat4 model;

out vec2 uv;

void main() {
    gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    uv = aUV;
}
)";
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 uv;
uniform sampler2D texture_diffuse;
uniform vec3 colorTint; // 可选的颜色混合

void main() {
    FragColor = texture(texture_diffuse, uv) * vec4(colorTint, 1.0);
}
)";
// renderer.cpp
void Renderer::loadTexture(const std::string &name, const std::string &path)
{
    m_Textures[name] = new Texture(path, window);
}

Texture *Renderer::getTexture(const std::string &name)
{
    return m_Textures.at(name);
}

Renderer::Renderer(int windowWidth, int windowHeight, GLFWwindow *window)
    : m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), window(window)
{
    glfwMakeContextCurrent(window);
    gl = (GladGLContext *)calloc(1, sizeof(GladGLContext));
    if (!gl)
    {
        throw std::invalid_argument("Failed to create context");
    }
    int version = gladLoadGLContext(gl, glfwGetProcAddress);
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
    InitShader();
    InitVertexData();
}

Renderer::~Renderer()
{
    gl->DeleteVertexArrays(1, &m_VAO);
    gl->DeleteBuffers(1, &m_VBO);
    gl->DeleteProgram(m_ShaderProgram);
}

void Renderer::InitShader()
{
    // 顶点着色器
    GLuint vertexShader = gl->CreateShader(GL_VERTEX_SHADER);
    gl->ShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    gl->CompileShader(vertexShader);

    // 检查编译错误
    GLint success;
    gl->GetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        gl->GetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "顶点着色器编译失败:\n"
                  << infoLog << std::endl;
    }

    // 片段着色器
    GLuint fragmentShader = gl->CreateShader(GL_FRAGMENT_SHADER);
    gl->ShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    gl->CompileShader(fragmentShader);

    // 检查编译错误
    gl->GetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        gl->GetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "片段着色器编译失败:\n"
                  << infoLog << std::endl;
    }

    // 链接着色器程序
    m_ShaderProgram = gl->CreateProgram();
    gl->AttachShader(m_ShaderProgram, vertexShader);
    gl->AttachShader(m_ShaderProgram, fragmentShader);
    gl->LinkProgram(m_ShaderProgram);

    // 检查链接错误
    gl->GetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        gl->GetProgramInfoLog(m_ShaderProgram, 512, NULL, infoLog);
        std::cerr << "着色器程序链接失败:\n"
                  << infoLog << std::endl;
    }

    // 清理
    gl->DeleteShader(vertexShader);
    gl->DeleteShader(fragmentShader);
}

void Renderer::InitVertexData()
{
    float vertices[] = {
        // 位置       // UV坐标
        -0.5f, -0.5f, 0.0f, 1.0f, // 左下
        0.5f, -0.5f, 1.0f, 1.0f,  // 右下
        0.5f, 0.5f, 1.0f, 0.0f,   // 右上
        -0.5f, 0.5f, 0.0f, 0.0f   // 左上
    };

    gl->GenVertexArrays(1, &m_VAO);
    gl->GenBuffers(1, &m_VBO);

    gl->BindVertexArray(m_VAO);
    gl->BindBuffer(GL_ARRAY_BUFFER, m_VBO);
    gl->BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 更新顶点属性指针
    gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    gl->EnableVertexAttribArray(0);
    gl->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    gl->EnableVertexAttribArray(1);

    gl->BindBuffer(GL_ARRAY_BUFFER, 0);
    gl->BindVertexArray(0);
}

void Renderer::SetProjection(int width, int height)
{
    m_WindowWidth = width;
    m_WindowHeight = height;
}

void Renderer::BeginBatch()
{
    gl->UseProgram(m_ShaderProgram);
    // 设置正交投影矩阵
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(m_WindowWidth),
        static_cast<float>(m_WindowHeight), 0.0f,
        -1.0f, 1.0f);
    gl->UniformMatrix4fv(gl->GetUniformLocation(m_ShaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
}

void Renderer::EndBatch()
{
    // 实际渲染调用
    gl->BindVertexArray(m_VAO);
    gl->DrawArrays(GL_TRIANGLE_FAN, 0, 4); // 绘制四边形
    gl->BindVertexArray(0);
}

void Renderer::DrawQuad(const glm::vec2 &position,
                        const glm::vec2 &size,
                        const std::string &textureName,
                        const glm::vec3 &colorTint)
{
    // 绑定纹理
    Texture *tex = getTexture(textureName);
    tex->bind(0);

    // 设置着色器参数
    GLint tintLoc = gl->GetUniformLocation(m_ShaderProgram, "colorTint");
    gl->Uniform3fv(tintLoc, 1, &colorTint[0]);

    // 原有模型矩阵计算
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    // 提交绘制命令
    // ...（同之前EndBatch逻辑）
    // 设置颜色和变换
    gl->UniformMatrix4fv(gl->GetUniformLocation(m_ShaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    // 绘制
    EndBatch(); // 实际提交绘制命令
}
