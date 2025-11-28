#pragma once

#include <functional>
#include <string>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width = 1920, int height = 1080,
        const std::string &title = "SceneLab");
    ~Window();

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    void initImGui();
    void cleanupImGui();

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();

    GLFWwindow *getGLFWWindow() const { return m_window; }

    // Callback management
    void addKeyCallback(int key, int action, std::function<void()> callback);
    void addCursorCallback(std::function<void(double, double)> callback);
    void addDropCallback(
        std::function<void(const std::vector<std::string> &paths,
            double mouseX, double mouseY)>
            callback);

private:
    GLFWwindow *m_window;
};
