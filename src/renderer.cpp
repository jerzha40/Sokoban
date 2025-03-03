#include <renderer.h>
#include <iostream>

const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform mat4 projection;
    uniform mat4 model; // 添加 model 矩阵
    void main()
    {
        gl_Position = projection * model * vec4(aPos, 0.0, 1.0); // 应用 model 矩阵
    }
)";
const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main()
    {
        FragColor = vec4(color, 1.0);
    }
)";

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
    // 四边形顶点数据（将被实例化）
    float vertices[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f,
        -0.5f, 0.5f};

    gl->GenVertexArrays(1, &m_VAO);
    gl->GenBuffers(1, &m_VBO);

    gl->BindVertexArray(m_VAO);
    gl->BindBuffer(GL_ARRAY_BUFFER, m_VBO);
    gl->BufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    gl->VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    gl->EnableVertexAttribArray(0);

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

void Renderer::DrawQuad(const glm::vec2 &position, const glm::vec2 &size, const glm::vec3 &color)
{
    // 计算模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    // 设置颜色和变换
    gl->Uniform3fv(gl->GetUniformLocation(m_ShaderProgram, "color"), 1, &color[0]);
    gl->UniformMatrix4fv(gl->GetUniformLocation(m_ShaderProgram, "model"), 1, GL_FALSE, &model[0][0]);

    // 绘制
    EndBatch(); // 实际提交绘制命令
}
