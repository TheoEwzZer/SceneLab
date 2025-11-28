#pragma once

#include "SceneGraph.hpp"
#include "TransformManager.hpp"
#include "renderer/implementation/RasterizationRenderer.hpp"

#include <glm/glm.hpp>
#include <string>

class TextureManager {
public:
    TextureManager(SceneGraph &sceneGraph, TransformManager &transformManager,
        RasterizationRenderer &renderer);

    void renderUI(bool *p_open = nullptr);

private:
    SceneGraph &m_sceneGraph;
    TransformManager &m_transformManager;
    RasterizationRenderer &m_renderer;

    int m_selectedTextureIndex = 0;
    glm::vec3 m_checkerColorA { 0.9f, 0.9f, 0.9f };
    glm::vec3 m_checkerColorB { 0.2f, 0.2f, 0.25f };
    int m_checkerChecks = 16;
    float m_noiseFrequency = 5.0f;
    glm::vec3 m_solidColor { 1.0f, 0.0f, 0.0f };
    bool m_useSRGB = true;

    void renderSelectionPanel();
    void renderTextureLibraryPanel();
    void renderToneMappingPanel();
    void renderCubemapPanel();

    void assignTextureToSelection(int textureHandle);
    void applyFilterToSelection(FilterMode mode);
};
