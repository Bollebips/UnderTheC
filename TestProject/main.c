#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#include <Containers/Array.h>
//#include <Logger.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <glfw3.h>
//#define GLFW_STATIC
//#include <GLFW/glfw3.h>

int width = 640;
int height = 480;

char* GetStringFromFile(const char* filePath)
{
    FILE* file = fopen(filePath, "rb");
    if(file == NULL)
    {
        //LogError("Unable to open file at path %s", filePath);
	printf("Unable to open file at path %s\n", filePath);
	return NULL;
    }

    fseek(file, 0L, SEEK_END);
    long length = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* result = (char*)malloc(length + 1);
    if(result != NULL)
    {
        fread(result, sizeof(char), length, file);
        result[length] = '\0'; 
        fclose(file);
    }
    else
    {
        //LogError("Memory allocation failed for file %s", filePath);
        printf("Memory allocation failed for file %s\n", filePath);
	return NULL;
    }

    return result;
}
void GlfwErrorCallback(int error, const char* message)
{
    //LogError("GLFW error: %s", message);
    printf("GLFW error: %s", message);
}

void GlfwWindowCloseCallback(GLFWwindow* window)
{
    //Do whatever when closing the window
}

void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //LogInfo("%d", key);
    printf("%d", key);

    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    glfwSetErrorCallback(GlfwErrorCallback);

    if(glfwInit() == false)
    {
        //LogError("Failed to initialize glfw.");
        printf("Failed to initialize glfw.");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, "My Title", NULL, NULL);
    //LogAssert(window != NULL, "GLFW window creation failed.");

    glfwMakeContextCurrent(window);

    glfwSetWindowCloseCallback(window, GlfwWindowCloseCallback);

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(-1, 0, width, height);

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

    const char* vertexShaderSource = GetStringFromFile("../vertexShader.glsl");
    const char* fragmentShaderSource = GetStringFromFile("../fragmentShader.glsl");

    GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint shaderCompileStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    char statusLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, statusLog);
    //LogInfo(statusLog);

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
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //LogInfo("Success!");
    printf("Success!");

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
