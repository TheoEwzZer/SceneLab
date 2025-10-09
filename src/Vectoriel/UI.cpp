#include "App.hpp"
#include "Vectoriel.hpp"
#include "imgui.h"
#include <glm/fwd.hpp>

using namespace Vect;

UIDrawer::UIDrawer() :
    m_outlineWidth(0.1f), m_outlineColor(0, 0, 0, 255),
    m_fillColor(0, 0, 0, 255), m_localScale(1.0f, 1.0f), m_fill(false),
    m_input_segments(5), m_circle_radius(1.0f)
{
}

void UIDrawer::renderUI(App *app)
{
    ImGui::Begin("Dessin vectoriel");

    ImGui::Separator();
    ImGui::Text("Primitives");

    const char *primitives[] = { "Ligne", "Triangle", "Carré", "Rectangle",
        "Polygone", "Cercle", "Ellipse", "Point" };
    static int current_primitive_idx = 0;
    ImGui::ListBox("Select primitive", &current_primitive_idx, primitives,
        IM_ARRAYSIZE(primitives));

    ImGui::Separator();
    ImGui::Text("Couleurs");
    ImGui::ColorEdit4("Couleur Contour", m_outlineColor);
    ImGui::ColorEdit4("Couleur Remplissage", m_fillColor);
    ImGui::Separator();

    ImGui::Text("Propriétés");
    ImGui::InputFloat("Taille Contour", &m_outlineWidth);
    ImGui::Checkbox("Remplissage", &m_fill);

    // TODO, faire ça en moins crad
    static int selection = 1;
    std::vector<float> tmp;

    switch (current_primitive_idx) {
        case 0:
            ImGui::InputFloat2("Position point A", m_line_pointA);
            ImGui::InputFloat2("Position point B", m_line_pointB);
            ImGui::InputFloat("Epaisseur", &m_line_width);
            ImGui::Separator();

            if (ImGui::Button("Generer Ligne")) {
                const auto &p = instanciatePrimitiveWAttributes<
                    Vect::Primitive::StraightLine>(
                    glm::vec2(m_line_pointA[0], m_line_pointA[1]),
                    glm::vec2(m_line_pointB[0], m_line_pointB[1]),
                    m_line_width);

                tmp = p.generateGLVertices();
            }
            break;
        case 1:
            if (ImGui::Button("Generer Triangle")) {
                const auto &p = instanciatePrimitiveWAttributes<
                    Vect::Primitive::Triangle>();
                tmp = p.generateGLVertices();
            }
            break;
        case 2:
            if (ImGui::Button("Taille")) {
                ImGui::InputFloat("Rayon", &m_circle_radius);
                const auto &p
                    = instanciatePrimitiveWAttributes<Vect::Primitive::Square>(
                        m_circle_radius);
                tmp = p.generateGLVertices();
            }
            break;
        case 3:
            ImGui::InputFloat("Facteur longueur", &m_localScale[0]);
            ImGui::InputFloat("Facteur largeur", &m_localScale[1]);
            if (ImGui::Button("Generer Rectangle")) {
                const auto &p = instanciatePrimitiveWAttributes<
                    Vect::Primitive::Rectangle>(
                    glm::vec2(m_localScale[0], m_localScale[1]));
                tmp = p.generateGLVertices();
            }
            break;
        case 4:
            ImGui::InputInt("Nb segments", &m_input_segments);
            if (ImGui::Button("Generer Polygone")) {
                const auto &p = instanciatePrimitiveWAttributes<
                    Vect::Primitive::RegularPolygon>(m_input_segments);
                tmp = p.generateGLVertices();
            }
            break;
        case 5:
            ImGui::InputFloat("Rayon", &m_circle_radius);
            if (ImGui::Button("Generer Cercle")) {
                const auto &p
                    = instanciatePrimitiveWAttributes<Vect::Primitive::Circle>(
                        m_circle_radius);
                tmp = p.generateGLVertices();
            }
            break;
        case 6:
            ImGui::InputFloat("Facteur longueur", &m_localScale[0]);
            ImGui::InputFloat("Facteur largeur", &m_localScale[1]);
            if (ImGui::Button("Generer Ellipse")) {
                const auto &p = instanciatePrimitiveWAttributes<
                    Vect::Primitive::Ellipse>(
                    glm::vec2(m_localScale[0], m_localScale[1]));
                tmp = p.generateGLVertices();
            }
            break;
        case 7:
            if (ImGui::Button("Generer Point")) {
                m_fill = true;
                const auto &p
                    = instanciatePrimitiveWAttributes<Vect::Primitive::Point>(
                        0.1f);
                tmp = p.generateGLVertices();
            }
            break;
        default:
            break;
    }
    if (tmp.size() > 0) {
        app->m_gameObjects.resize(app->m_gameObjects.size() + 1);
        app->selectedObjectIndex = app->m_gameObjects[++selection].rendererId
            = app->m_renderer->registerObject(tmp, {}, "", false);
    }

    // if (ImGui::Button("Ajoute Cercle")) {
    //     const auto &c
    //         = instanciatePrimitiveWAttributes<Vect::Primitive::Circle>(3);
    //     renderer->registerObject(c.generateGLVertices(), {}, "", false);
    // }

    // if (ImGui::Button("Ajoute Rectangle")) {
    //     const auto &c
    //         = instanciatePrimitiveWAttributes<Vect::Primitive::Rectangle>();
    //     renderer->registerObject(c.generateGLVertices(), {}, "", false);
    // }

    // if (ImGui::Button("Ajoute Carre")) {
    //     const auto &c
    //         = instanciatePrimitiveWAttributes<Vect::Primitive::Square>(3);
    //     renderer->registerObject(c.generateGLVertices(), {}, "", false);
    // }

    // if (ImGui::Button("Ajoute Triangle")) {
    //     const auto &c
    //         = instanciatePrimitiveWAttributes<Vect::Primitive::Triangle>();
    //     renderer->registerObject(c.generateGLVertices(), {}, "", false);
    // }

    // if (ImGui::Button("Ajoute Polygone")) {
    //     int n = 12;
    //     const auto &c
    //         =
    //         instanciatePrimitiveWAttributes<Vect::Primitive::RegularPolygon>(12);
    //     renderer->registerObject(c.generateGLVertices(), {}, "", false);
    // }

    ImGui::End();
}

UIDrawer::~UIDrawer() {}