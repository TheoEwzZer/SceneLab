#pragma once

#include "GameObject.hpp"
#include "SceneGraph.hpp"
#include "GeometryImguiWindow.hpp"
#include "renderer/interface/IRenderer.hpp"
#include <memory>
#include <functional>
#include <string>

class GeometryManager {
public:
    GeometryManager(
        SceneGraph &sceneGraph, std::unique_ptr<IRenderer> &renderer);

    // Initialize geometry window callbacks
    void initGeometryWindow(std::function<void()> onObjectCreated);

    // Render geometry UI
    void renderUI(bool *p_open = nullptr);

    // Access to the window for external configuration
    GeometryImguiWindow &getGeometryWindow() { return m_geometryWindow; }

private:
    SceneGraph &m_sceneGraph;
    std::unique_ptr<IRenderer> &m_renderer;
    GeometryImguiWindow m_geometryWindow;
};
