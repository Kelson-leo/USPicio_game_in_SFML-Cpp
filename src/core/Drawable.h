#pragma once

namespace core {

class IRenderer; // fwd

/// Abstract drawable — anything that can be rendered.
class Drawable {
public:
    virtual ~Drawable() = default;
    virtual void draw(IRenderer& renderer) const = 0;
};

} // namespace core
