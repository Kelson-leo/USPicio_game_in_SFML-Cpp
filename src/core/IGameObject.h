#pragma once

namespace core {

/// Port: any entity that participates in the game loop.
class IGameObject {
public:
    virtual ~IGameObject() = default;

    /// Called at a fixed rate (e.g. 60 Hz).
    virtual void fixedUpdate(float deltaTime) = 0;

    /// Called every frame (variable rate).
    virtual void update(float deltaTime) = 0;

    /// Draw the object.
    virtual void render(class IRenderer& renderer) = 0;
};

} // namespace core
