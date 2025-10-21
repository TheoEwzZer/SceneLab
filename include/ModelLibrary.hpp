#ifndef MODELLIBRARY_H
#define MODELLIBRARY_H

#include "GeometryGenerator.hpp"

#include <cstddef>
#include <string>
#include <map>
#include <optional>

struct ModelEntry {
    std::string name;
    std::string filepath;
    GData data;
    std::size_t instanceCount { 0 };
};

class ModelLibrary {
public:
    void addModel(const std::string &name, const std::string &filepath,
        const GData &data);

    void removeModel(const std::string &filepath);

    [[nodiscard]] const std::map<std::string, ModelEntry> &getModels() const
    {
        return m_models;
    }

    [[nodiscard]] std::size_t getInstanceCount(
        const std::string &filepath) const;

    void incrementInstanceCount(const std::string &filepath);

    [[nodiscard]] std::optional<std::reference_wrapper<const GData>>
    getModelData(const std::string &filepath) const;

private:
    std::map<std::string, ModelEntry> m_models;
};

#endif /* MODELLIBRARY_H */
