#pragma once

#include <cstdlib>
#include <functional>
#include <glad/gl.h>
#include <string>
#include <vector>
#define GLFW_INCLUDE_NONE
#include "CameraManager.hpp"

#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <glm/glm.hpp>

class ARenderer {
public:
    ARenderer();
    virtual ~ARenderer();

    ARenderer(const ARenderer &) = delete;
    ARenderer &operator=(const ARenderer &) = delete;

    void init();

    // Object Related
    virtual int registerObject(const std::vector<float> &vertices,
        const std::vector<unsigned int> &indices,
        const std::string &texturePath, bool isLight)
        = 0;
    virtual void updateTransform(int objectId, const glm::mat4 &modelMatrix)
        = 0;
    virtual void removeObject(int objectId) = 0;

    // Camera Related
    virtual void setViewMatrix(const glm::mat4 &view) = 0;
    virtual void setProjectionMatrix(const glm::mat4 &proj) = 0;

    // Rendering Related
    virtual void drawAll() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    void createCameraViews(int id, int width = 512, int height = 512);
    void destroyCameraViews(int id);
    void renderAllViews(CameraManager& cameraManager);

    // Abstract
    virtual bool shouldWindowClose();
    void addKeyCallback(int key, int action, std::function<void()> callback);
    void addCursorCallback(std::function<void(double, double)> callback);

protected:
    struct CameraView {
        unsigned int fbo = 0;
        unsigned int colorTex = 0;
        unsigned int depthRBO = 0;
        glm::ivec2 size = {512, 512};
        std::string name = "camera";
    };

    std::unordered_map<int, CameraView> m_cameraViews;

    void renderCameraViews(const Camera & cam, const CameraView & view);
    void renderDockableViews(CameraManager &cameraManager);
    GLFWwindow *m_window;

private:
};
