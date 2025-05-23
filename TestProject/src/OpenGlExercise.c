#include "OpenGlExercise.h"

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <Logger.h>
#include <Utils/FileIO.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

static void GlfwErrorCallback(int error, const char* message)
{
    LogError("GLFW error: %s", message);
}

static void GlfwWindowCloseCallback(GLFWwindow* window)
{
    //Do whatever when closing the window
}

static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

static void GlfwFrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int Exercise()
{
    glfwSetErrorCallback(GlfwErrorCallback);

    if(glfwInit() == false)
    {
        LogError("Failed to initialize glfw.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    int width = 640;
    int height = 480;
    GLFWwindow* window = glfwCreateWindow(width, height, "My Title", NULL, NULL);
    LogAssert(window != NULL, "GLFW window creation failed.");

    glfwMakeContextCurrent(window);

    glfwSetWindowCloseCallback(window, GlfwWindowCloseCallback);
    glfwSetFramebufferSizeCallback(window, GlfwFrameBufferSizeCallback);
    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, GlfwKeyCallback);

    glewExperimental = GL_TRUE;
    GLenum glewInitResult = glewInit();
    LogAssert(glewInitResult == GLEW_OK, "Glew did not initialize correctly: %s\n", glewGetErrorString(glewInitResult));

    const char* vertexShaderSource = GetStringFromFile("vertexShader.glsl");
    const char* fragmentShaderSource = GetStringFromFile("fragmentShader.glsl");
    const char* fragmentShader2Source = GetStringFromFile("fragmentShader2.glsl");

    LogAssert(vertexShaderSource != NULL, "VertexShaderSource didn't load correctly.");
    LogAssert(fragmentShaderSource != NULL, "FragmentShaderSource didn't load correctly.");

    char statusLog[512];
    GLuint shaderCompileStatus;

    //Compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    free((char*) vertexShaderSource);
    glCompileShader(vertexShader);
    
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    if(shaderCompileStatus == false)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, statusLog);
        LogError("VertextShader compilation failed: %s", statusLog);
    }

    //Compile fragmentShader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    free((char*) fragmentShaderSource);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderCompileStatus);
    if(shaderCompileStatus == false)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, statusLog);
        LogError("FragmentShader compilation failed: %s", statusLog);
    }

    //Compile fragmentShader
    GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShader2Source, NULL);
    free((char*) fragmentShader2Source);
    glCompileShader(fragmentShader2);

    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &shaderCompileStatus);
    if(shaderCompileStatus == false)
    {
        glGetShaderInfoLog(fragmentShader2, 512, NULL, statusLog);
        LogError("FragmentShader2 compilation failed: %s", statusLog);
    }

    GLint linkStatus;

    //Create program and link shaders
    GLint shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader);
    glLinkProgram(shaderProgram1);
    glDeleteShader(fragmentShader);

    glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == false)
    {
       glGetProgramInfoLog(shaderProgram1, 512, NULL, statusLog);
       LogError("Shader program 1 linking failed: %s", statusLog);
    }

    GLint shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    glDeleteShader(fragmentShader2);

    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == false)
    {
       glGetProgramInfoLog(shaderProgram2, 512, NULL, statusLog);
       LogError("Shader program 2 linking failed: %s", statusLog);
    }

    glDeleteShader(vertexShader);

    float rectangle1[] =
    {
        0.9f, 0.5f, 0.0f, //top right
        0.9f, -0.5f, 0.0f,//bottom right
        0.6f, -0.5f, 0.0f,//bottom left
        0.6f, 0.5f, 0.0f//top left
    };

    float rectangle2[] =
    {
        -0.6f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, //top right
        -0.6f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,//bottom right
        -0.9f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,//bottom left
        -0.9f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f//top left
    };

    uint indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    //Create vertex erray object
    GLuint vaos[2];
    glGenVertexArrays(2, (GLuint*) &vaos);

    //Create vertex buffer object, bind it, and add the vertices to it
    GLuint vbos[2];
    glGenBuffers(2, (GLuint*) &vbos);

    //Create element buffer object
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //Link the position attribute from the vertex shader to the vbo currently bound to GL_ARRAY_BUFFER
    GLint positionAttribute = glGetAttribLocation(shaderProgram1, "position");
    GLint customColorAttribute = glGetUniformLocation(shaderProgram1, "customColor");

    glBindVertexArray(vaos[0]);
    //bind the ebo, which gets bound to the current vao as well, which is vaos[0]
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
    //Specify the "position" attributes, at the location, 3 floats, no normalization, and 12 bytes (3 * float) stride, 0 offset
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(positionAttribute);

    positionAttribute = glGetAttribLocation(shaderProgram2, "position");
    GLint vertexColorAttribute = glGetAttribLocation(shaderProgram2, "inColor");

    glBindVertexArray(vaos[1]);
    //bind the ebo, which gets bound to the current vao as well, which is vaos[1]
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
    //Specify the "position" attributes, at the location, 3 floats, no normalization, and 12 bytes (3 * float) stride, 0 offset
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(vertexColorAttribute, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(vertexColorAttribute);

    /* glBindFragDataLocation(shaderProgram, 0, "outColor"); */

    while (!glfwWindowShouldClose(window))
    { 
        float time = glfwGetTime();
        float greenValue = (sin(time * 10) / 2.0f) + 0.5f;

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shaderProgram1);
        glUniform4f(customColorAttribute, 0.0f, greenValue, 0.0f, 1.0f);

        glBindVertexArray(vaos[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle1), rectangle1, GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgram2);

        glBindVertexArray(vaos[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle2), rectangle2, GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    LogInfo("Success!");

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
