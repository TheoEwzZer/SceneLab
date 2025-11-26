#pragma once

#include <cstdint>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include "TransformManager.hpp"
#include "objects/Light.hpp"
#include "GameObject.hpp"
#include "renderer/interface/ARenderer.hpp"

class App;

namespace Illumination {
/**
 * @brief Classe de l'interface graphique du dessin vectoriel
 */
class UIIllumination {
public:
    UIIllumination(TransformManager &tref);
    ~UIIllumination();

    /**
     * @brief Fonction de rendu de l'interface graphique.
     * @param app Pointeur sur l'application
     */
    void renderUI(App *app);

    void setCurrentColorRGBA(const glm::vec4 &rgba, bool applyFill = true,
        bool applyOutline = true);

protected:
    void renderUIIllumination(App *app);

    int m_illumination_model = 0;
    float m_shininess = 10.0f;
    float m_ambiant_color[3] = {0.1,0.1,0.1};
    float m_diffuse_color[3] = {0.3,0.3,0.3};
    float m_specular_color[3] = {0.5,0.5,0.5};
    float m_emissive_color[3] = {0,0,0};

    TransformManager &m_tref;

    // Persisted UI state

};

}
