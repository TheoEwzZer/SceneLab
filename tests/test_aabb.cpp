/**
 * @file test_aabb.cpp
 * @brief Tests unitaires pour la structure AABB (Axis-Aligned Bounding Box)
 *
 * Teste les opérations sur les bounding boxes : expansion, centroid,
 * extent, surface area, et détection de l'axe le plus long.
 */

#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <limits>

#include "renderer/BVH.hpp"

class AABBTest : public ::testing::Test {
protected:
    AABB box;
};

TEST_F(AABBTest, DefaultValuesAreExtreme)
{
    EXPECT_GT(box.min.x, 0.0f);
    EXPECT_LT(box.max.x, 0.0f);
}

TEST_F(AABBTest, ExpandWithPoint)
{
    box.expand(glm::vec3(1.0f, 2.0f, 3.0f));

    EXPECT_EQ(box.min.x, 1.0f);
    EXPECT_EQ(box.min.y, 2.0f);
    EXPECT_EQ(box.min.z, 3.0f);
    EXPECT_EQ(box.max.x, 1.0f);
    EXPECT_EQ(box.max.y, 2.0f);
    EXPECT_EQ(box.max.z, 3.0f);
}

TEST_F(AABBTest, ExpandWithMultiplePoints)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(2.0f, 4.0f, 6.0f));
    box.expand(glm::vec3(-1.0f, -2.0f, -3.0f));

    EXPECT_EQ(box.min.x, -1.0f);
    EXPECT_EQ(box.min.y, -2.0f);
    EXPECT_EQ(box.min.z, -3.0f);
    EXPECT_EQ(box.max.x, 2.0f);
    EXPECT_EQ(box.max.y, 4.0f);
    EXPECT_EQ(box.max.z, 6.0f);
}

TEST_F(AABBTest, ExpandWithAABB)
{
    AABB other;
    other.expand(glm::vec3(-5.0f, -5.0f, -5.0f));
    other.expand(glm::vec3(5.0f, 5.0f, 5.0f));

    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(other);

    EXPECT_EQ(box.min.x, -5.0f);
    EXPECT_EQ(box.min.y, -5.0f);
    EXPECT_EQ(box.min.z, -5.0f);
    EXPECT_EQ(box.max.x, 5.0f);
    EXPECT_EQ(box.max.y, 5.0f);
    EXPECT_EQ(box.max.z, 5.0f);
}

TEST_F(AABBTest, CentroidOfSymmetricBox)
{
    box.expand(glm::vec3(-1.0f, -1.0f, -1.0f));
    box.expand(glm::vec3(1.0f, 1.0f, 1.0f));

    glm::vec3 centroid = box.centroid();

    EXPECT_NEAR(centroid.x, 0.0f, 0.0001f);
    EXPECT_NEAR(centroid.y, 0.0f, 0.0001f);
    EXPECT_NEAR(centroid.z, 0.0f, 0.0001f);
}

TEST_F(AABBTest, CentroidOfAsymmetricBox)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(4.0f, 6.0f, 8.0f));

    glm::vec3 centroid = box.centroid();

    EXPECT_NEAR(centroid.x, 2.0f, 0.0001f);
    EXPECT_NEAR(centroid.y, 3.0f, 0.0001f);
    EXPECT_NEAR(centroid.z, 4.0f, 0.0001f);
}

TEST_F(AABBTest, ExtentCalculation)
{
    box.expand(glm::vec3(-1.0f, -2.0f, -3.0f));
    box.expand(glm::vec3(2.0f, 4.0f, 6.0f));

    glm::vec3 extent = box.extent();

    EXPECT_NEAR(extent.x, 3.0f, 0.0001f);  // 2 - (-1)
    EXPECT_NEAR(extent.y, 6.0f, 0.0001f);  // 4 - (-2)
    EXPECT_NEAR(extent.z, 9.0f, 0.0001f);  // 6 - (-3)
}

TEST_F(AABBTest, ExtentOfPointBox)
{
    box.expand(glm::vec3(5.0f, 5.0f, 5.0f));

    glm::vec3 extent = box.extent();

    EXPECT_NEAR(extent.x, 0.0f, 0.0001f);
    EXPECT_NEAR(extent.y, 0.0f, 0.0001f);
    EXPECT_NEAR(extent.z, 0.0f, 0.0001f);
}

TEST_F(AABBTest, SurfaceAreaOfUnitCube)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(1.0f, 1.0f, 1.0f));

    float area = box.surfaceArea();

    EXPECT_NEAR(area, 6.0f, 0.0001f);
}

TEST_F(AABBTest, SurfaceAreaOfRectangularBox)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(2.0f, 3.0f, 4.0f));

    float area = box.surfaceArea();

    // 2*(2*3 + 3*4 + 4*2) = 2*(6 + 12 + 8) = 52
    EXPECT_NEAR(area, 52.0f, 0.0001f);
}

TEST_F(AABBTest, SurfaceAreaOfFlatBox)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(2.0f, 2.0f, 0.0f));

    float area = box.surfaceArea();

    // Box plate : 2*(2*2 + 2*0 + 0*2) = 2*4 = 8
    EXPECT_NEAR(area, 8.0f, 0.0001f);
}

TEST_F(AABBTest, LongestAxisX)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(10.0f, 5.0f, 3.0f));

    EXPECT_EQ(box.longestAxis(), 0);  // X
}

TEST_F(AABBTest, LongestAxisY)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(3.0f, 10.0f, 5.0f));

    EXPECT_EQ(box.longestAxis(), 1);  // Y
}

TEST_F(AABBTest, LongestAxisZ)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(3.0f, 5.0f, 10.0f));

    EXPECT_EQ(box.longestAxis(), 2);  // Z
}

TEST_F(AABBTest, LongestAxisCube)
{
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(5.0f, 5.0f, 5.0f));

    EXPECT_EQ(box.longestAxis(), 2);
}

TEST(AABBNegativeTest, NegativeCoordinates)
{
    AABB box;
    box.expand(glm::vec3(-10.0f, -20.0f, -30.0f));
    box.expand(glm::vec3(-5.0f, -10.0f, -15.0f));

    glm::vec3 centroid = box.centroid();
    EXPECT_NEAR(centroid.x, -7.5f, 0.0001f);
    EXPECT_NEAR(centroid.y, -15.0f, 0.0001f);
    EXPECT_NEAR(centroid.z, -22.5f, 0.0001f);

    glm::vec3 extent = box.extent();
    EXPECT_NEAR(extent.x, 5.0f, 0.0001f);
    EXPECT_NEAR(extent.y, 10.0f, 0.0001f);
    EXPECT_NEAR(extent.z, 15.0f, 0.0001f);
}

TEST(AABBEdgeCaseTest, VerySmallBox)
{
    AABB box;
    box.expand(glm::vec3(0.0f, 0.0f, 0.0f));
    box.expand(glm::vec3(0.0001f, 0.0001f, 0.0001f));

    EXPECT_GT(box.surfaceArea(), 0.0f);
    EXPECT_LT(box.surfaceArea(), 0.001f);
}

TEST(AABBEdgeCaseTest, VeryLargeBox)
{
    AABB box;
    box.expand(glm::vec3(-1000.0f, -1000.0f, -1000.0f));
    box.expand(glm::vec3(1000.0f, 1000.0f, 1000.0f));

    glm::vec3 extent = box.extent();
    EXPECT_NEAR(extent.x, 2000.0f, 0.0001f);

    // Surface area = 2 * (2000*2000 + 2000*2000 + 2000*2000) = 24,000,000
    EXPECT_NEAR(box.surfaceArea(), 24000000.0f, 1.0f);
}
