#include <stdio.h>

#include <Containers/Array.h>
#include <Logger.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

int width = 640;
int height = 480;

// const char* vertexShaderSource = "glsl(#version 150 core\nin vec2 position;void main(){gl_Position = vec4(position, 0.0, 1.0);})glsl";
// const char* fragmentShaderSource = "glsl(#version 150 core out vec4 outColor;void main(){outColor = vec4(1.1, 1.0, 1.0, 1.0);})glsl";

char* GetStringFromFile(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if(file == NULL)
    {
        LogError("Unable to open file at path %s", filePath);
    }

    fseek(file, 0L, SEEK_END);
    long length = ftell(file);
    fseek(file, 0L, SEEK_SET);

char* result = malloc(length + 1);
if(result != NULL)
{
fread(result, sizeof(char), length, file);
fclose(file);
}
else
{
    LogError("Memory allocation failed for file %s", filePath);
}
    return result;
}
void GlfwErrorCallback(int error, const char* message)
{
    LogError("GLFW error: %s", message);
}

void GlfwWindowCloseCallback(GLFWwindow* window)
{
    LogWarning("Closing window");
}

void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    LogInfo("%d", key);

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(GlfwErrorCallback);

    if(glfwInit() == false)
    {
        LogError("WOOPS");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "My Title", NULL, NULL);
    LogAssert(window != NULL, "GLFW window creation failed.");

    glfwMakeContextCurrent(window);

    glfwSetWindowCloseCallback(window, GlfwWindowCloseCallback);

    glfwGetFramebufferSize(window, &width, &height);
    // glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, GlfwKeyCallback);

    glewExperimental = GL_TRUE;
    glewInit();

    float vertices[] =
    {
        0.0f, 0.5f,
        0.5f, -0.5f,
        -0.5f, -0.5f
    };

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const char* vertexShaderSource = GetStringFromFile("bin/vertexShader.glsl");
    const char* fragmentShaderSource = GetStringFromFile("bin/fragmentShader.glsl");

LogInfo(vertexShaderSource);
LogInfo(fragmentShaderSource);

    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint shaderCompileStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    char statusLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, statusLog);
    LogInfo(statusLog);

    GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    GLint positionAttribute = glGetAttribLocation(shaderProgram, "position");

    glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(positionAttribute);


    while (!glfwWindowShouldClose(window))
    {
        // render(window);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    LogInfo("Succes!");

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
