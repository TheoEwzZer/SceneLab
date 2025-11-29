//
// Created by clmonn on 11/27/25.
//

#ifndef SCENELAB_CURVE_H
#define SCENELAB_CURVE_H

#include "RenderableObject.hpp"

class DynamicLine : public RenderableObject {
    public:
    explicit DynamicLine(glm::vec3 color = glm::vec3(1.0f));

    void draw(const ShaderProgram &vectorial,
              const ShaderProgram &pointLight,
              const ShaderProgram &lighting,
              const TextureLibrary &textures) const override;
};

#endif // SCENELAB_CURVE_H
