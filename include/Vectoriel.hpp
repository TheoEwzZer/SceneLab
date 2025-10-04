#pragma once

#include <cstdint>
#include <glm/fwd.hpp>
#include <string>
#include <vector>
#include "GameObject.hpp"

struct RGBAColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

class ASimpleVectPrimitive : public GameObject {
public:
    ASimpleVectPrimitive();
    virtual ~ASimpleVectPrimitive() {};

    void setOutlineWidth(float width);
    float getOutlineWidth() const;
    void setOutlineColor(const RGBAColor &color);
    RGBAColor getOutlineColor() const;
    void setFillColor(const RGBAColor &color);
    RGBAColor getFillColor() const;

    void setFilled(bool fill);
    bool isFilled();
    std::string getType() const;

    void setLocalScale(glm::vec2 scale);
    glm::vec2 getLocalScale() const;

    void setLocalRotation(float angle);
    float getLocalRotation() const;


    virtual std::vector<float> generateGLVertices() const = 0;

protected:
    std::string m_type;

    bool m_filled;
    RGBAColor m_fillColor;

    RGBAColor m_outlineColor;
    float m_outlineWidth;

    glm::vec2 m_scale;
    float m_rotator_offset;
};

class VectPolygon : public ASimpleVectPrimitive {
public:
    VectPolygon(uint32_t segments);
    ~VectPolygon();

    std::vector<float> generateGLVertices() const override;

protected:
    uint32_t m_segments;

    // Structure pour les jointures
    struct MiterVertices {
        glm::vec2 outer;
        glm::vec2 inner;
    };

    static MiterVertices calculateMiterJoint(
        glm::vec2 prev, glm::vec2 curr, glm::vec2 next, float halfWidth);
};

class VectEllipse : public VectPolygon
{
public:
    VectEllipse(glm::vec2 radius, uint32_t resolution = 100);
    ~VectEllipse();

    void setResolution(uint32_t resolution);
    uint32_t getResolution() const;
};

class VectCircle : public VectEllipse
{
public:
    VectCircle(float radius, uint32_t resolution = 100);
    ~VectCircle();
};

class VectRectangle : public VectPolygon
{
public:
    VectRectangle(glm::vec2 size);
    ~VectRectangle();
};

class VectSquare : public VectRectangle
{
public:
    VectSquare(float size);
    ~VectSquare();
};

