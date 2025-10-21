//
// Created by clmonn on 10/10/25.
//

#ifndef SCENELAB_CAMERAMANAGER_H
#define SCENELAB_CAMERAMANAGER_H

#include "Camera.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class CameraManager {
    public:
    explicit CameraManager() = default;
    int getCameraCount() const;
    int createCamera();
    void destroyCamera(int id);
    void setFocused(int id);
    Camera* getCamera(int id);
    Camera* getFocusedCamera();
    const Camera *getFocusedCamera() const;

    void setPosition(const glm::vec3& position);
    void setRotation(float pitch, float yaw, float roll);
    void setProjection(float fov, float aspectRatio, float nearPlane, float farPlane);
    glm::vec3 getPosition() const;
    glm::vec3 getRotation() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    private:
    std::unordered_map<int, Camera> m_cameras;
    int m_nextID = 1;
    std::optional<int> m_FocusedCamID;
};

#endif // SCENELAB_CAMERAMANAGER_H