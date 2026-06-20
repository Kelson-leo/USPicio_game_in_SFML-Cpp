#pragma once

#include <string>

namespace core {

/// Port: abstracts texture loading from disk.
/// Will be used by AssetManager in a later sprint.
class ITextureLoader {
public:
    virtual ~ITextureLoader() = default;

    /// Load texture from a file path.
    virtual bool loadFromFile(const std::string& path) = 0;

    /// Dimensions of the currently loaded texture.
    virtual unsigned int getWidth()  const = 0;
    virtual unsigned int getHeight() const = 0;
};

} // namespace core
