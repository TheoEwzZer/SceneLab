#include <gtest/gtest.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <cmath>

#include "GameObject.hpp"

bool vec3Equal(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f)
{
    return glm::all(glm::epsilonEqual(a, b, epsilon));
}

bool mat4Equal(const glm::mat4& a, const glm::mat4& b, float epsilon = 0.0001f)
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (std::abs(a[i][j] - b[i][j]) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

class GameObjectTest : public ::testing::Test {
protected:
    GameObject obj;
};

TEST_F(GameObjectTest, DefaultPosition)
{
    EXPECT_TRUE(vec3Equal(obj.getPosition(), glm::vec3(0.0f)));
}

TEST_F(GameObjectTest, DefaultRotation)
{
    EXPECT_TRUE(vec3Equal(obj.getRotation(), glm::vec3(0.0f)));
}

TEST_F(GameObjectTest, DefaultScale)
{
    EXPECT_TRUE(vec3Equal(obj.getScale(), glm::vec3(1.0f)));
}

TEST_F(GameObjectTest, DefaultRendererId)
{
    EXPECT_EQ(obj.rendererId, -1);
}

TEST_F(GameObjectTest, SetPosition)
{
    glm::vec3 newPos(1.0f, 2.0f, 3.0f);
    obj.setPosition(newPos);
    EXPECT_TRUE(vec3Equal(obj.getPosition(), newPos));
}

TEST_F(GameObjectTest, SetRotation)
{
    glm::vec3 newRot(0.5f, 1.0f, 1.5f);
    obj.setRotation(newRot);
    EXPECT_TRUE(vec3Equal(obj.getRotation(), newRot));
}

TEST_F(GameObjectTest, SetScale)
{
    glm::vec3 newScale(2.0f, 3.0f, 4.0f);
    obj.setScale(newScale);
    EXPECT_TRUE(vec3Equal(obj.getScale(), newScale));
}

TEST_F(GameObjectTest, SetName)
{
    obj.setName("TestObject");
    EXPECT_STREQ(obj.m_name, "TestObject");
}

TEST_F(GameObjectTest, SetNameTruncation)
{
    obj.setName("ThisIsAVeryLongNameThatShouldBeTruncated");
    EXPECT_LE(strlen(obj.m_name), 25);
}

TEST_F(GameObjectTest, TransformDirtyAfterSetPosition)
{
    obj.getLocalMatrix();
    EXPECT_FALSE(obj.isTransformDirty());

    obj.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_TRUE(obj.isTransformDirty());
}

TEST_F(GameObjectTest, TransformDirtyAfterSetRotation)
{
    obj.getLocalMatrix();
    EXPECT_FALSE(obj.isTransformDirty());

    obj.setRotation(glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_TRUE(obj.isTransformDirty());
}

TEST_F(GameObjectTest, TransformDirtyAfterSetScale)
{
    obj.getLocalMatrix();
    EXPECT_FALSE(obj.isTransformDirty());

    obj.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    EXPECT_TRUE(obj.isTransformDirty());
}

TEST_F(GameObjectTest, NoChangeNoDirty)
{
    obj.getLocalMatrix();
    EXPECT_FALSE(obj.isTransformDirty());

    obj.setPosition(glm::vec3(0.0f));
    EXPECT_FALSE(obj.isTransformDirty());
}

TEST_F(GameObjectTest, IdentityMatrixAtDefault)
{
    const glm::mat4& localMat = obj.getLocalMatrix();
    EXPECT_TRUE(mat4Equal(localMat, glm::mat4(1.0f)));
}

TEST_F(GameObjectTest, TranslationMatrix)
{
    obj.setPosition(glm::vec3(5.0f, 10.0f, 15.0f));
    const glm::mat4& localMat = obj.getLocalMatrix();

    glm::mat4 expected = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 10.0f, 15.0f));
    EXPECT_TRUE(mat4Equal(localMat, expected));
}

TEST_F(GameObjectTest, ScaleMatrix)
{
    obj.setScale(glm::vec3(2.0f, 3.0f, 4.0f));
    const glm::mat4& localMat = obj.getLocalMatrix();

    glm::mat4 expected = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 4.0f));
    EXPECT_TRUE(mat4Equal(localMat, expected));
}

TEST_F(GameObjectTest, CombinedTransform)
{
    obj.setPosition(glm::vec3(1.0f, 2.0f, 3.0f));
    obj.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
    const glm::mat4& localMat = obj.getLocalMatrix();

    glm::mat4 expected = glm::mat4(1.0f);
    expected = glm::translate(expected, glm::vec3(1.0f, 2.0f, 3.0f));
    expected = glm::scale(expected, glm::vec3(2.0f, 2.0f, 2.0f));

    EXPECT_TRUE(mat4Equal(localMat, expected));
}

TEST_F(GameObjectTest, WorldMatrixWithParent)
{
    glm::mat4 parentMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f));
    obj.setPosition(glm::vec3(5.0f, 0.0f, 0.0f));

    const glm::mat4& worldMat = obj.getWorldMatrix(parentMatrix);

    glm::vec4 transformedOrigin = worldMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_NEAR(transformedOrigin.x, 15.0f, 0.0001f);
}

TEST_F(GameObjectTest, SetAABB)
{
    glm::vec3 corner1(-1.0f, -2.0f, -3.0f);
    glm::vec3 corner2(1.0f, 2.0f, 3.0f);

    obj.setAABB(corner1, corner2);

    EXPECT_TRUE(vec3Equal(obj.getAABBCorner1(), corner1));
    EXPECT_TRUE(vec3Equal(obj.getAABBCorner2(), corner2));
}

TEST_F(GameObjectTest, BoundingBoxActiveDefault)
{
    EXPECT_FALSE(obj.isBoundingBoxActive());
}

TEST_F(GameObjectTest, SetBoundingBoxActive)
{
    obj.setBoundingBoxActive(true);
    EXPECT_TRUE(obj.isBoundingBoxActive());

    obj.setBoundingBoxActive(false);
    EXPECT_FALSE(obj.isBoundingBoxActive());
}

TEST_F(GameObjectTest, HelperDefault)
{
    EXPECT_FALSE(obj.isHelper());
}

TEST_F(GameObjectTest, SetHelper)
{
    obj.setHelper(true);
    EXPECT_TRUE(obj.isHelper());
}

TEST_F(GameObjectTest, HasMovedAfterPositionChange)
{
    obj.setHasMoved(false);
    obj.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_TRUE(obj.hasTransformChanged());
}

TEST_F(GameObjectTest, ResetHasMoved)
{
    obj.setPosition(glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_TRUE(obj.hasTransformChanged());

    obj.setHasMoved(false);
    EXPECT_FALSE(obj.hasTransformChanged());
}
