#include <stdio.h>

#include <Containers/Array.h>
#include <Logger.h>

#include <GLFW/glfw3.h>\

int width = 640;
int height = 480;

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

    if(key == GLFW_KEY_ESCAPE)
    {
        glfwSetWindowShouldClose(window, true);
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "My Title", NULL, NULL);
    LogAssert(window != NULL, "GLFW window creation failed.");

    glfwMakeContextCurrent(window);

    glfwSetWindowCloseCallback(window, GlfwWindowCloseCallback);

    glfwGetFramebufferSize(window, &width, &height);
    // glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, GlfwKeyCallback);

    while (!glfwWindowShouldClose(window))
    {
        // render(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    LogInfo("Succes!");

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}