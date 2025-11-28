#include "renderer/Window.hpp"

#include <cstdlib>
#include <functional>
#include <glad/gl.h>
#include <unordered_map>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include "ImGuizmo.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// Callback maps
static std::unordered_map<int,
    std::unordered_map<int, std::vector<std::function<void()>>>>
    keyCallbacks = {};
static std::function<void(double, double)> cursorCallback = nullptr;
static std::function<void(const std::vector<std::string> &, double, double)>
    dropCallback;

static void error_callback(int error, const char *description)
{
    (void)error;
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Forward to ImGui first
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    for (const auto &callback : keyCallbacks[key][action]) {
        callback();
    }
}

static void mouse_callback(
    GLFWwindow *window, int button, int action, int mods)
{
    // Forward to ImGui first
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

    for (const auto &callback : keyCallbacks[button][action]) {
        callback();
    }
}

static void cursor_callback(GLFWwindow *window, double x, double y)
{
    // Forward to ImGui first
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);

    if (cursorCallback) {
        cursorCallback(x, y);
    }
}

static void drop_callback(GLFWwindow *window, int count, const char **paths)
{
    double mx = 0.0, my = 0.0;
    if (window) {
        glfwGetCursorPos(window, &mx, &my);
    }
    if (!dropCallback) {
        return;
    }
    std::vector<std::string> dropped;
    dropped.reserve(static_cast<size_t>(count));
    for (int i = 0; i < count; ++i) {
        dropped.emplace_back(paths[i] ? paths[i] : "");
    }
    dropCallback(dropped, mx, my);
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

static void char_callback(GLFWwindow *window, unsigned int c)
{
    ImGui_ImplGlfw_CharCallback(window, c);
}

Window::Window(int width, int height, const std::string &title)
{
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!m_window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(m_window, key_callback);
    glfwSetMouseButtonCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetCharCallback(m_window, char_callback);

    glfwMakeContextCurrent(m_window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}

Window::~Window()
{
    cleanupImGui();

    if (m_window) {
        glfwDestroyWindow(m_window);
    }

    glfwTerminate();
}

void Window::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, false);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Window::cleanupImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }

void Window::swapBuffers() { glfwSwapBuffers(m_window); }

void Window::pollEvents() { glfwPollEvents(); }

void Window::addKeyCallback(
    int key, int action, std::function<void()> callback)
{
    keyCallbacks[key][action].push_back(callback);
}

void Window::addCursorCallback(std::function<void(double, double)> callback)
{
    cursorCallback = callback;
    glfwSetCursorPosCallback(m_window, cursor_callback);
}

void Window::addDropCallback(std::function<void(
        const std::vector<std::string> &paths, double mouseX, double mouseY)>
        callback)
{
    dropCallback = std::move(callback);
    glfwSetDropCallback(m_window, drop_callback);
}
