#pragma once

#include <cstdint>
#include <vector>

static constexpr int ROWS = 5;
static constexpr int COLS = 8;

enum class BallType : uint8_t {
    Red,
    Green,
    Blue,
    Yellow,
};

enum class BallState : uint8_t {
    None = 0,
    EnteringFromNorth,
    EnteringFromEast,
    EnteringFromSouth,
    EnteringFromWest,
    ExitingTowardsNorth,
    ExitingTowardsEast,
    ExitingTowardsSouth,
    ExitingTowardsWest,
    InsideRotor,
};

struct Ball {
    BallState state = BallState::None;
    BallType type;
    double transition;
    int row;
    int col;
    int rotor_position;
};

enum class TileType : uint8_t {
    Empty,
    CornerNorthEast,
    CornerNorthWest,
    CornerSouthEast,
    CornerSouthWest,
    Horizontal,
    Vertical,
    Crossing,
    Rotor,
};

enum class RotorState : uint8_t {
    Resting,
    TurningClockwise,
    TurningCounterClockwise,
};

struct Tile {
    TileType type;
    union {
        struct {
            RotorState state;
            int position;
            double transition;
        } rotor;
    };
};

class Model {
public:
    void turnClockwise(int row, int col);
    void turnCounterClockwise(int row, int col);

    void progress(double milliseconds);

    const std::vector<Ball> &balls() const { return _balls; }
    std::vector<Ball> &balls() { return _balls; }

    const std::vector<Tile> &tiles() const { return _tiles; }
    std::vector<Tile> &tiles() { return _tiles; }

    const Tile &tile(int row, int col) const { return _tiles[col + row * COLS]; }
    Tile &tile(int row, int col) { return _tiles[col + row * COLS]; }

private:
    std::vector<Tile> _tiles;
    std::vector<Ball> _balls;
};
