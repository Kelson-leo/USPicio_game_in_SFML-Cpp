#pragma once

namespace core {

enum class Direction {
    Left,
    Right
};

inline Direction opposite(Direction d) {
    return (d == Direction::Right) ? Direction::Left : Direction::Right;
}

} // namespace core
