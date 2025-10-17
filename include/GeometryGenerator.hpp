/*
** IFT3100A25
** GeometryGenerator
** File description:
** Utility class for generating 3D geometric primitives procedurally
*/

#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include <vector>
#include <glm/glm.hpp>

// The returned vertex format is : pox.x | pox.y | pos.z | text.u | test.v |
// n.x | n.y | n.z

class GeometryGenerator {
public:
    // sectors = vertical splits
    // subdivisions = horizontal splits
    static std::vector<float> generateSphere(
        float radius, int sectors, int stacks);

    static std::vector<float> generateCube(float size);

    // sectors = vertical splits
    static std::vector<float> generateCylinder(
        float radius, float height, int sectors);

private:
    // Helper function to add a vertex to the vertex data vector
    static void addVertex(std::vector<float> &vertices,
        const glm::vec3 &position, const glm::vec2 &texCoord,
        const glm::vec3 &normal);
};

#endif /* GEOMETRYGENERATOR_H */
