#include "ModelLibrary.hpp"

void ModelLibrary::addModel(
    const std::string &name, const std::string &filepath, const GData &data)
{
    if (m_models.contains(filepath)) [[unlikely]] {
        return;
    }

    m_models[filepath] = { name, filepath, data, 0 };
}

void ModelLibrary::removeModel(const std::string &filepath)
{
    m_models.erase(filepath);
}

[[nodiscard]] std::size_t ModelLibrary::getInstanceCount(
    const std::string &filepath) const
{
    if (!m_models.contains(filepath)) [[unlikely]] {
        return 0;
    }

    return m_models.at(filepath).instanceCount;
}

void ModelLibrary::incrementInstanceCount(const std::string &filepath)
{
    if (m_models.contains(filepath)) {
        m_models.at(filepath).instanceCount++;
    }
}

[[nodiscard]] std::optional<std::reference_wrapper<const GData>>
ModelLibrary::getModelData(const std::string &filepath) const
{
    const auto it = m_models.find(filepath);
    if (it != m_models.end()) {
        return std::cref(it->second.data);
    }

    return std::nullopt;
}
