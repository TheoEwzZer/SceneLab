#include "GeometryImguiWindow.hpp"
#include "imgui.h"

void GeometryImguiWindow::render()
{
    if (!m_isOpen) {
        return;
    }

    ImGui::Begin("Spawn new primitive", &m_isOpen);

    ImGui::SeparatorText("Sphere");
    ImGui::SliderFloat("Radius", &m_sphereRadius, 0.1f, 2.0f);
    ImGui::SliderInt("Sectors (vertical slices)", &m_sphereSectors, 3, 64);
    ImGui::SliderInt("Stacks (horizontal slices)", &m_sphereStacks, 2, 32);
    if (ImGui::Button("Spawn Sphere") && onSpawnSphere) {
        onSpawnSphere(m_sphereRadius, m_sphereSectors, m_sphereStacks);
        m_sphereCount++;
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Cube");
    ImGui::SliderFloat("Cube Size", &m_cubeSize, 0.1f, 2.0f);
    if (ImGui::Button("Spawn Cube") && onSpawnCube) {
        onSpawnCube(m_cubeSize);
        m_cubeCount++;
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Cylinder");
    ImGui::SliderFloat("Cylinder Radius", &m_cylinderRadius, 0.1f, 2.0f);
    ImGui::SliderFloat("Height", &m_cylinderHeight, 0.1f, 5.0f);
    ImGui::SliderInt("Cylinder Sectors", &m_cylinderSectors, 3, 64);
    if (ImGui::Button("Spawn Cylinder") && onSpawnCylinder) {
        onSpawnCylinder(m_cylinderRadius, m_cylinderHeight, m_cylinderSectors);
        m_cylinderCount++;
    }

    ImGui::End();
}
