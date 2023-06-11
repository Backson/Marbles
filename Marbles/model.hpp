#pragma once

#include <cstdint>
#include <vector>

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
            bool taken[4];
            bool connected[4];
        } rotor;
    };
};

class Model {
public:
    Model(int rows, int cols);

    void clear();

    void turnClockwise(int row, int col);
    void turnCounterClockwise(int row, int col);

    void eject(int row, int col, int direction);
    void ejectNorth(int row, int col) { eject(row, col, 0); }
    void ejectEast(int row, int col) { eject(row, col, 1); }
    void ejectSouth(int row, int col) { eject(row, col, 2); }
    void ejectWest(int row, int col) { eject(row, col, 3); }

    void progress(double milliseconds);

    int rows() const { return _rows; }
    int cols() const { return _cols; }

    const std::vector<Ball> &balls() const { return _balls; }
    std::vector<Ball> &balls() { return _balls; }

    const std::vector<Tile> &tiles() const { return _tiles; }
    std::vector<Tile> &tiles() { return _tiles; }

    const Tile &tile(int row, int col) const { return _tiles[col + row * _cols]; }
    Tile &tile(int row, int col) { return _tiles[col + row * _cols]; }

private:
    int _rows;
    int _cols;
    std::vector<Tile> _tiles;
    std::vector<Ball> _balls;
};
