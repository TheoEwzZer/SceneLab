//
// Created by clmonn on 10/10/25.
//

#include "../include/CameraManager.hpp"

#include <ranges>

int CameraManager::getCameraCount() const { return m_cameras.size(); }

int CameraManager::createCamera()
{
    const int id = m_nextID++;
    m_cameras[id] = Camera {};
    return id;
}

void CameraManager::destroyCamera(const int id)
{
    m_cameras.erase(id);

    if (m_FocusedCamID == id) {
        m_FocusedCamID.reset();
    }
}

void CameraManager::setFocused(int id) { m_FocusedCamID = id; }

Camera *CameraManager::getCamera(const int id)
{
    const auto it = m_cameras.find(id);
    return (it != m_cameras.end()) ? &it->second : nullptr;
}

Camera *CameraManager::getFocusedCamera()
{
    if (!m_FocusedCamID) {
        return nullptr;
    }
    const auto it = m_cameras.find(*m_FocusedCamID);
    return (it != m_cameras.end()) ? &it->second : nullptr;
}

const Camera *CameraManager::getFocusedCamera() const
{
    if (!m_FocusedCamID) {
        return nullptr;
    }
    const auto it = m_cameras.find(*m_FocusedCamID);
    return (it != m_cameras.end()) ? &it->second : nullptr;
}

void CameraManager::setPosition(const glm::vec3 &position)
{
    if (auto *cam = getFocusedCamera()) {
        cam->setPosition(position);
    }
}

void CameraManager::setRotation(
    const float pitch, const float yaw, const float roll)
{
    if (auto *cam = getFocusedCamera()) {
        cam->setRotation(pitch, yaw, roll);
    }
}

void CameraManager::setProjection(const float fov, const float aspectRatio,
    const float nearPlane, const float farPlane)
{
    if (auto *cam = getFocusedCamera()) {
        cam->setProjection(fov, aspectRatio, nearPlane, farPlane);
    }
}

glm::vec3 CameraManager::getPosition() const
{
    if (auto *cam = getFocusedCamera()) {
        return cam->getPosition();
    }
    return glm::vec3(0, 0, 0);
}

glm::vec3 CameraManager::getRotation() const
{
    if (auto *cam = getFocusedCamera()) {
        return cam->getRotation();
    }
    return glm::vec3(0, 0, 0);
}

glm::mat4 CameraManager::getViewMatrix() const
{
    if (auto *cam = getFocusedCamera()) {
        return cam->getViewMatrix();
    }
    return glm::mat4(1.0f);
}

glm::mat4 CameraManager::getProjectionMatrix() const
{
    if (auto *cam = getFocusedCamera()) {
        return cam->getProjectionMatrix();
    }
    return glm::mat4(1.0f);
}