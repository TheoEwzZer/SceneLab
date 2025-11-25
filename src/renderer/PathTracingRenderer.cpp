#include "renderer/implementation/PathTracingRenderer.hpp"
#include "ShaderProgram.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ImGuizmo.h"
#include "imgui.h"
#include "objects/Object3D.hpp"
#include "renderer/interface/IRenderer.hpp"

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include "objects/Light.hpp"

PathTracingRenderer::PathTracingRenderer(Window &window)
    : m_window(window)
{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    m_pathTracingShader.init(
        "../assets/shaders/shader.vert",
        "../assets/shaders/pathtracing.frag");
    m_pathTracingShader.use();
    glm::mat4 identity = glm::mat4(1.0f);
    m_pathTracingShader.setMat4("model", identity);
    m_pathTracingShader.setMat4("view", identity);
    m_pathTracingShader.setMat4("projection", identity);
    m_pathTracingShader.setVec3("viewPos", glm::vec3(0.0f, 0.0f, 4.0f));

    // Initialize view and projection matrices
    m_viewMatrix = glm::mat4(1.0f);
    m_viewMatrix = glm::translate(m_viewMatrix, glm::vec3(0.0f, 0.0f, -4.0f));
    // default to orthographic projection
    m_projMatrix = glm::ortho(
        -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    float quadVertices[] = {
        // positions        // texCoords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f  // top right
    };
    // register in VBO, VAO etc.
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);

    initAccumulationBuffers();
}

PathTracingRenderer::~PathTracingRenderer()
{
    cleanupAccumulationBuffers();
}

int PathTracingRenderer::registerObject(std::unique_ptr<RenderableObject> obj)
{
    (void) obj;
    return 0;
}

int PathTracingRenderer::registerObject(std::unique_ptr<RenderableObject> obj, const std::string &texturePath)
{
    (void) texturePath;
    (void) obj;
    return 0;
}

int PathTracingRenderer::registerObject(std::unique_ptr<RenderableObject> obj, const glm::vec3 &color)
{
    (void) color;
    (void) obj;
    return 0;
}


void PathTracingRenderer::updateTransform(
    const int objectId, const glm::mat4 &modelMatrix)
{
    (void) objectId;
    (void) modelMatrix;
}

void PathTracingRenderer::removeObject(const int objectId)
{
    if (objectId < 0 || objectId >= static_cast<int>(m_renderObjects.size()))
        return;

    if (!m_renderObjects[objectId])
        return;

    m_renderObjects[objectId].reset();

    m_freeSlots.push_back(objectId);
}

std::vector<std::unique_ptr<RenderableObject>> PathTracingRenderer::extractAllObjects()
{
    std::vector<std::unique_ptr<RenderableObject>> objects;
    return objects;
}

void PathTracingRenderer::beginFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pathTracingShader.use();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags
        = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

    ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0, 0, 1920, 1080);
}

void PathTracingRenderer::drawAll(Camera cam)
{
}

void PathTracingRenderer::endFrame()
{
    m_iFrame++;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window.swapBuffers();
    m_window.pollEvents();
}

// Window-related methods
bool PathTracingRenderer::shouldWindowClose()
{
    return m_window.shouldClose();
}

void PathTracingRenderer::addKeyCallback(
    int key, int action, std::function<void()> callback)
{
    m_window.addKeyCallback(key, action, callback);
}

void PathTracingRenderer::addCursorCallback(
    std::function<void(double, double)> callback)
{
    m_window.addCursorCallback(callback);
}

void PathTracingRenderer::addDropCallback(
    std::function<void(const std::vector<std::string> &paths,
        double mouseX, double mouseY)>
        callback)
{
    m_window.addDropCallback(callback);
}

GLFWwindow *PathTracingRenderer::getWindow() const
{
    return m_window.getGLFWWindow();
}

// Camera view management methods
void PathTracingRenderer::setCameraOverlayCallback(CameraOverlayCallback callback)
{
    m_cameraOverlayCallback = std::move(callback);
}

void PathTracingRenderer::renderAllViews(CameraManager &cameraManager)
{
    for (auto &[id, view] : m_cameraViews) {
        if (auto *cam = cameraManager.getCamera(id)) {
            renderCameraViews(*cam, view);
        }
    }
    renderDockableViews(cameraManager);
    // Unlock when mouse released
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        m_lockCameraWindows = false;
        m_lockedCameraId = -1;
    }
}

void PathTracingRenderer::createCameraViews(const int id, int width, int height)
{
    if (!m_cameraViews.contains(id)) {
        CameraView view;
        view.size = { width, height };

        // Create and bind framebuffer
        glGenFramebuffers(1, &view.fbo);

        glBindFramebuffer(GL_FRAMEBUFFER, view.fbo);

        // Create and attach color texture
        glGenTextures(1, &view.colorTex);
        glBindTexture(GL_TEXTURE_2D, view.colorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, view.colorTex, 0);

        // Create and attach depth-stencil buffer
        glGenRenderbuffers(1, &view.depthRBO);
        glBindRenderbuffer(GL_RENDERBUFFER, view.depthRBO);
        glRenderbufferStorage(
            GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, view.depthRBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
            != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Framebuffer is incomplete!" << std::endl;
        }

        // Initialize accumulation buffers for this camera
        initCameraAccumulationBuffers(view);

        m_cameraViews[id] = std::move(view);
    }
}

void PathTracingRenderer::destroyCameraViews(const int id)
{
    if (const auto it = m_cameraViews.find(id); it != m_cameraViews.end()) {
        auto &view = it->second;
        glDeleteFramebuffers(1, &view.fbo);
        glDeleteTextures(1, &view.colorTex);
        glDeleteRenderbuffers(1, &view.depthRBO);
        cleanupCameraAccumulationBuffers(view);
        m_cameraViews.erase(it);
    }
}

void PathTracingRenderer::renderCameraViews(const Camera &cam, CameraView &view)
{
    // Save current state
    GLint previousFBO;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO);
    GLint previousViewport[4];
    glGetIntegerv(GL_VIEWPORT, previousViewport);

    // Check if we need to reset accumulation (camera moved)
    if (shouldResetCameraAccumulation(cam, view)) {
        resetCameraAccumulation(view);
        view.lastViewPos = cam.getPosition();
        view.lastViewRotation = cam.getRotation();
    }

    // Bind the current accumulation FBO to render to
    glBindFramebuffer(GL_FRAMEBUFFER, view.accumulationFBO[view.currentAccumulationBuffer]);
    glViewport(0, 0, view.size.x, view.size.y);

    // Render pathtraced content
    m_pathTracingShader.use();
    m_pathTracingShader.setVec3("viewPos", cam.getPosition());
    glm::vec3 rotationRadians = glm::radians(cam.getRotation());
    m_pathTracingShader.setVec3("viewRotation", rotationRadians);
    float aspectRatio = (view.size.y > 0) ? (static_cast<float>(view.size.x) / static_cast<float>(view.size.y)) : 1.0f;
    m_pathTracingShader.setFloat("aspectRatio", aspectRatio);
    m_pathTracingShader.setInt("iFrame", view.iFrame);

    // Bind the previous frame texture for accumulation
    int previousBuffer = 1 - view.currentAccumulationBuffer;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view.accumulationTexture[previousBuffer]);
    m_pathTracingShader.setInt("previousFrame", 0);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Now copy to the display framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, view.fbo);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, view.accumulationTexture[view.currentAccumulationBuffer]);
    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Swap buffers for next frame
    view.currentAccumulationBuffer = 1 - view.currentAccumulationBuffer;
    view.iFrame++;

    // Restore previous state
    glBindFramebuffer(GL_FRAMEBUFFER, previousFBO);
    glViewport(previousViewport[0], previousViewport[1], previousViewport[2],
        previousViewport[3]);
}

void PathTracingRenderer::renderDockableViews(CameraManager &cameraManager)
{
    for (auto &[id, view] : m_cameraViews) {
        const std::string name = "Camera " + std::to_string(id);
        ImGui::SetNextWindowSize(ImVec2(512, 512), ImGuiCond_FirstUseEver);

        ImGuiWindowFlags windowFlags = 0;
        if (m_lockCameraWindows && m_lockedCameraId == id) {
            windowFlags |= ImGuiWindowFlags_NoMove;
            windowFlags |= ImGuiWindowFlags_NoResize;
            windowFlags |= ImGuiWindowFlags_NoScrollbar;
            windowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
            if (view.hasState) {
                ImGui::SetNextWindowPos(view.lastPos);
                ImGui::SetNextWindowSize(view.lastSize);
            }
        }

        ImGui::Begin(name.c_str(), nullptr, windowFlags);

        // Controls toolbar for this camera
        if (auto *cam = cameraManager.getCamera(id)) {
            ImGui::PushID(id);
            const std::string tableId
                = std::string("cam_ctl_") + std::to_string(id);
            if (ImGui::BeginTable(
                    tableId.c_str(), 5, ImGuiTableFlags_SizingFixedFit)) {
                ImGui::TableNextColumn();
                bool isPerspective = cam->getProjectionMode()
                    == Camera::ProjectionMode::Perspective;
                if (ImGui::Checkbox("Persp##mode", &isPerspective)) {
                    cam->setProjectionMode(isPerspective
                            ? Camera::ProjectionMode::Perspective
                            : Camera::ProjectionMode::Orthographic);
                }

                ImGui::TableNextColumn();
                if (isPerspective) {
                    float fov = cam->getFov();
                    if (ImGui::DragFloat(
                            "FOV##fov", &fov, 0.1f, 10.0f, 160.0f, "%.1f")) {
                        cam->setFov(fov);
                    }
                } else {
                    float orthoSize = cam->getOrthoSize();
                    if (ImGui::DragFloat("Size##ortho", &orthoSize, 0.05f,
                            0.01f, 100.0f, "%.2f")) {
                        cam->setOrthoSize(orthoSize);
                    }
                }

                ImGui::TableNextColumn();

                ImGui::TableNextColumn();
                if (ImGui::SmallButton("Reset Pose##reset")) {
                    cam->setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                    cam->setRotation(0.0f, 0.0f, 0.0f);
                }
                ImGui::EndTable();
            }
            ImGui::PopID();
        }

        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImVec2 windowPos = ImGui::GetWindowPos();
        int newW = static_cast<int>(avail.x);
        int newH = static_cast<int>(avail.y);
        if (newW < 2 || newH < 2) {
            newW = view.size.x;
            newH = view.size.y;
        }
        if (view.size.x != newW || view.size.y != newH) {
            view.size = { newW, newH };

            glBindFramebuffer(GL_FRAMEBUFFER, view.fbo);

            glBindTexture(GL_TEXTURE_2D, view.colorTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, avail.x, avail.y, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glBindRenderbuffer(GL_RENDERBUFFER, view.depthRBO);
            glRenderbufferStorage(
                GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, avail.x, avail.y);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Recreate accumulation buffers with new size
            cleanupCameraAccumulationBuffers(view);
            initCameraAccumulationBuffers(view);

            if (auto *cam = cameraManager.getCamera(id)) {
                const float aspect = static_cast<float>(view.size.x)
                    / static_cast<float>(view.size.y);
                cam->setAspect(aspect);
            }
        }
        ImVec2 imagePos = ImGui::GetCursorScreenPos();
        ImGui::Image((void *)(intptr_t)view.colorTex, avail, ImVec2(0, 1),
            ImVec2(1, 0));

        // Auto focus this camera when user clicks on its image/window
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left)
            || (ImGui::IsWindowHovered()
                && ImGui::IsMouseClicked(ImGuiMouseButton_Left))) {
            cameraManager.setFocused(id);
        }

        // Record state for locking
        view.lastPos = windowPos;
        view.lastSize = ImGui::GetWindowSize();
        view.hasState = true;

        bool isHovered = ImGui::IsItemHovered();

        if (m_cameraOverlayCallback) {
            if (const auto *cam = cameraManager.getCamera(id)) {
                m_cameraOverlayCallback(id, *cam, imagePos, avail, isHovered);
            }
        }

        if (ImGuizmo::IsUsing() && isHovered) {
            m_lockCameraWindows = true;
            m_lockedCameraId = id;
        }

        ImGui::End();
    }
}

void PathTracingRenderer::initAccumulationBuffers()
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    int width = viewport[2];
    int height = viewport[3];

    if (width <= 0 || height <= 0) {
        width = 1920;
        height = 1080;
    }

    // Create two framebuffers and textures for ping-pong
    glGenFramebuffers(2, m_accumulationFBO);
    glGenTextures(2, m_accumulationTexture);

    for (int i = 0; i < 2; i++) {
        // Setup texture
        glBindTexture(GL_TEXTURE_2D, m_accumulationTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Setup framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_accumulationFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_accumulationTexture[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Accumulation framebuffer " << i << " is incomplete!" << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear both textures to black
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_accumulationFBO[i]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PathTracingRenderer::cleanupAccumulationBuffers()
{
    glDeleteFramebuffers(2, m_accumulationFBO);
    glDeleteTextures(2, m_accumulationTexture);
}

void PathTracingRenderer::resetAccumulation()
{
    m_iFrame = 0;

    // Clear both accumulation buffers
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_accumulationFBO[i]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool PathTracingRenderer::shouldResetAccumulation(const Camera &cam) const
{
    const float epsilon = 0.0001f;

    glm::vec3 currentPos = cam.getPosition();
    glm::vec3 currentRot = cam.getRotation();

    // Check if camera position or rotation has changed
    if (glm::length(currentPos - m_lastViewPos) > epsilon) {
        return true;
    }

    if (glm::length(currentRot - m_lastViewRotation) > epsilon) {
        return true;
    }

    return false;
}

void PathTracingRenderer::initCameraAccumulationBuffers(CameraView &view)
{
    int width = view.size.x;
    int height = view.size.y;

    if (width <= 0 || height <= 0) {
        width = 512;
        height = 512;
    }

    // Create two framebuffers and textures for ping-pong
    glGenFramebuffers(2, view.accumulationFBO);
    glGenTextures(2, view.accumulationTexture);

    for (int i = 0; i < 2; i++) {
        // Setup texture
        glBindTexture(GL_TEXTURE_2D, view.accumulationTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Setup framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, view.accumulationFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, view.accumulationTexture[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Camera accumulation framebuffer " << i << " is incomplete!" << std::endl;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Clear both textures to black
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, view.accumulationFBO[i]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PathTracingRenderer::cleanupCameraAccumulationBuffers(CameraView &view)
{
    if (view.accumulationFBO[0] != 0 || view.accumulationFBO[1] != 0) {
        glDeleteFramebuffers(2, view.accumulationFBO);
        view.accumulationFBO[0] = 0;
        view.accumulationFBO[1] = 0;
    }
    if (view.accumulationTexture[0] != 0 || view.accumulationTexture[1] != 0) {
        glDeleteTextures(2, view.accumulationTexture);
        view.accumulationTexture[0] = 0;
        view.accumulationTexture[1] = 0;
    }
}

void PathTracingRenderer::resetCameraAccumulation(CameraView &view)
{
    view.iFrame = 0;

    // Clear both accumulation buffers
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, view.accumulationFBO[i]);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool PathTracingRenderer::shouldResetCameraAccumulation(const Camera &cam, CameraView &view) const
{
    const float epsilon = 0.0001f;

    glm::vec3 currentPos = cam.getPosition();
    glm::vec3 currentRot = cam.getRotation();

    // Check if camera position or rotation has changed
    if (glm::length(currentPos - view.lastViewPos) > epsilon) {
        return true;
    }

    if (glm::length(currentRot - view.lastViewRotation) > epsilon) {
        return true;
    }

    return false;
}
