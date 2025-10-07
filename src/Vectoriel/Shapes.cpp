#include "Vectoriel.hpp"
#include <cmath>
#include <cstdint>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <vector>

namespace Vect::Shape {

House::House() : AShape()
{
    auto square = std::make_unique<Vect::Primitive::Square>(0.5f);
    auto triangle = std::make_unique<Vect::Primitive::RegularPolygon>(3);
    auto rect
        = std::make_unique<Vect::Primitive::Rectangle>(glm::vec2(0.1, 0.22));
    triangle->setLocalPosition({ 0, 0.5 });
    triangle->setLocalScale({ 0.55, 0.35f });
    triangle->setLocalRotation(-30);
    rect->setLocalPosition({ 0.28, 0.5 });

    m_primitives.emplace_back(std::move(square));
    m_primitives.emplace_back(std::move(triangle));
    m_primitives.emplace_back(std::move(rect));
}

House::~House() {};

Doll::Doll() : AShape()
{
    auto rect
        = std::make_unique<Vect::Primitive::Rectangle>(glm::vec2(0.3, 0.1));
    auto triangle = std::make_unique<Vect::Primitive::Triangle>();
    auto circle = std::make_unique<Vect::Primitive::Circle>(0.30f);
    auto triangle2 = std::make_unique<Vect::Primitive::Triangle>();

    triangle->setLocalPosition({0, 0.75});
    // triangle->setLocalRotation(-30);
    triangle->setLocalScale({0.4,0.25});

    circle->setLocalPosition({0, 0.35});

    triangle2->setLocalPosition({0, -0.60});
    // triangle2->setLocalRotation(-30);
    triangle2->setLocalScale({0.55,0.55});

    m_primitives.emplace_back(std::move(triangle));
    m_primitives.emplace_back(std::move(circle));
    m_primitives.emplace_back(std::move(rect));
    m_primitives.emplace_back(std::move(triangle2));
}

Doll::~Doll() {};

}