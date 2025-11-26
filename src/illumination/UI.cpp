#include "App.hpp"
#include "illumination/Illumination.hpp"
#include "imgui.h"
#include "objects/Material.hpp"
#include "renderer/implementation/RasterizationRenderer.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>


using namespace Illumination;

UIIllumination::UIIllumination(TransformManager &tref) :
    m_illumination_model(0), m_tref(tref)
{
}


void UIIllumination::renderUI(App *app)
{
    static const char *models[] = { "Lambert", "Phong", "Blinn-Phong", "Gouraud" };
    static const char *lights[] = { "Directional", "Point", "Spot" };

    ImGui::Begin("Illumination");

    ImGui::Separator();
    ImGui::Text("Scene illumination");
    if (ImGui::Combo("Model", &m_illumination_model, models,
        IM_ARRAYSIZE(models))) {
        app->m_rasterRenderer->setLightingModel(static_cast<ARenderer::LightingModel>(m_illumination_model));
    }

    ImGui::Separator();
    ImGui::Text("Material");

    ImGui::ColorEdit3("Ambiant Color", m_ambiant_color);
    ImGui::ColorEdit3("Diffuse Color", m_diffuse_color);
    ImGui::ColorEdit3("Specular Color", m_specular_color);
    ImGui::ColorEdit3("Emissive Color", m_emissive_color);
    ImGui::SliderFloat("Shininess", &m_shininess, 1.0f, 150.0f);

    ImGui::Separator();
    ImGui::Text("Light");


    for (auto *node : m_tref.getSelectedNodes()) {
        if (!node) {
            continue;
        }
        int rendererId = node->getData().rendererId;
        if (rendererId >= 0) {
            RasterizationRenderer *rend = dynamic_cast<RasterizationRenderer *>(app->m_renderer.get());
            if (rend != nullptr) {
                Material mat(glm::vec3(m_ambiant_color[0], m_ambiant_color[1], m_ambiant_color[2]),
                glm::vec3(m_diffuse_color[0], m_diffuse_color[1], m_diffuse_color[2]),
                glm::vec3(m_specular_color[0], m_specular_color[1], m_specular_color[2]),
                glm::vec3(m_emissive_color[0], m_emissive_color[1], m_emissive_color[2]), m_shininess);
                rend->assignMaterialToObject(rendererId, mat);
            }
         }
    }

    ImGui::End();
}

UIIllumination::~UIIllumination() {}
