#ifndef SCENELAB_MATERIAL_H
#define SCENELAB_MATERIAL_H

#include "ShaderProgram.hpp"
#include "glm/glm.hpp"

struct Material {
    Material(const glm::vec3 &ambientColor = {0.2f,0.2f,0.2f}, const glm::vec3 diffuseColor = {1.0f,1.0f,1.0f},
        const glm::vec3 specularColor = {1.0f,1.0f,1.0f}, const glm::vec3 emissiveColor = {0.0f,0.0f,0.0f},
        float shininess = 0.1f) :
        m_ambientColor(ambientColor), m_diffuseColor(diffuseColor),
        m_specularColor(specularColor), m_emissiveColor(emissiveColor),
        m_shininess(shininess) {};
    ~Material() {};

    glm::vec3 m_ambientColor;
    glm::vec3 m_diffuseColor;
    glm::vec3 m_specularColor;
    glm::vec3 m_emissiveColor;
    float m_shininess;

    static void setShaderUniforms(const ShaderProgram &shader, const Material &mat)
    {
        // shader.setVec3("objectMaterial.ambient", mat.m_ambientColor);
        // shader.setVec3("objectMaterial.diffuse", mat.m_ambientColor);
        // shader.setVec3("objectMaterial.specular", mat.m_ambientColor);
        // shader.setVec3("objectMaterial.emissive", mat.m_ambientColor);
        // shader.setFloat("objectMaterial.shininess", mat.m_shininess);

        shader.setVec3("objectMaterial.ambient", {0.0,0.0,0.0});
        shader.setVec3("objectMaterial.diffuse", {0.5,0.5,0.5});
        shader.setVec3("objectMaterial.specular", {0.5,0.5,0.5});
        shader.setVec3("objectMaterial.emissive", {0.0,0.0,0.0});
        shader.setFloat("objectMaterial.shininess", 0.5);
    }
};

#endif // SCENELAB_MATERIAL_H
