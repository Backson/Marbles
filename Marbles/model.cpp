#include "model.hpp"

Model::Model(int rows, int cols) :
    _rows(rows), _cols(cols)
{
    _tiles.resize(rows * cols);
    clear();
}

void Model::clear() {
    for (auto &tile : _tiles) {
        tile = Tile{ TileType::Empty };
    }
}

void Model::turnClockwise(int row, int col) {
    if (tile(row, col).type == TileType::Rotor && tile(row, col).rotor.state == RotorState::Resting) {
        tile(row, col).rotor.state = RotorState::TurningClockwise;
        tile(row, col).rotor.transition = 0.0;
    }
}

void Model::turnCounterClockwise(int row, int col) {
    if (tile(row, col).type == TileType::Rotor && tile(row, col).rotor.state == RotorState::Resting) {
        tile(row, col).rotor.state = RotorState::TurningCounterClockwise;
        tile(row, col).rotor.transition = 0.0;
    }
}

static void progress_rotor(Tile &tile, double milliseconds) {
    if (tile.rotor.state == RotorState::TurningClockwise)
    {
        tile.rotor.transition += milliseconds / 1000.0 * 60.0 / 5.0;
        if (tile.rotor.transition >= 1.0) {
            tile.rotor.state = RotorState::Resting;
            tile.rotor.position += 1;
            tile.rotor.position %= 4;
        }
    }
    else if (tile.rotor.state == RotorState::TurningCounterClockwise)
    {
        tile.rotor.transition += milliseconds / 1000.0 * 60.0 / 5.0;
        if (tile.rotor.transition >= 1.0) {
            tile.rotor.state = RotorState::Resting;
            tile.rotor.position += 4;
            tile.rotor.position -= 1;
            tile.rotor.position %= 4;
        }
    }
}

static void progress_ball(Ball &ball, const Tile &tile, double milliseconds) {
    // tiles per millisecond
    double velocity = 1e-3;
    ball.transition += milliseconds * velocity;

    switch (ball.state) {
    case BallState::EnteringFromNorth:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            switch (tile.type) {
            case TileType::CornerNorthEast:
                ball.state = BallState::ExitingTowardsEast;
                break;
            case TileType::CornerNorthWest:
                ball.state = BallState::ExitingTowardsWest;
                break;
            case TileType::Vertical:
            case TileType::Crossing:
                ball.state = BallState::ExitingTowardsSouth;
                break;
            default:
                ball.state = BallState::ExitingTowardsNorth;
                break;
            }
        }
        break;

    case BallState::EnteringFromEast:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            switch (tile.type) {
            case TileType::CornerNorthEast:
                ball.state = BallState::ExitingTowardsNorth;
                break;
            case TileType::CornerSouthEast:
                ball.state = BallState::ExitingTowardsSouth;
                break;
            case TileType::Horizontal:
            case TileType::Crossing:
                ball.state = BallState::ExitingTowardsWest;
                break;
            default:
                ball.state = BallState::ExitingTowardsEast;
                break;
            }
        }
        break;

    case BallState::EnteringFromSouth:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            switch (tile.type) {
            case TileType::CornerSouthEast:
                ball.state = BallState::ExitingTowardsEast;
                break;
            case TileType::CornerSouthWest:
                ball.state = BallState::ExitingTowardsWest;
                break;
            case TileType::Vertical:
            case TileType::Crossing:
                ball.state = BallState::ExitingTowardsNorth;
                break;
            default:
                ball.state = BallState::ExitingTowardsSouth;
                break;
            }
        }
        break;

    case BallState::EnteringFromWest:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            switch (tile.type) {
            case TileType::CornerNorthWest:
                ball.state = BallState::ExitingTowardsNorth;
                break;
            case TileType::CornerSouthWest:
                ball.state = BallState::ExitingTowardsSouth;
                break;
            case TileType::Horizontal:
            case TileType::Crossing:
                ball.state = BallState::ExitingTowardsEast;
                break;
            default:
                ball.state = BallState::ExitingTowardsWest;
                break;
            }
        }
        break;

    case BallState::ExitingTowardsNorth:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.row -= 1;
            ball.state = BallState::EnteringFromSouth;
        }
        break;

    case BallState::ExitingTowardsEast:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.col += 1;
            ball.state = BallState::EnteringFromWest;
        }
        break;

    case BallState::ExitingTowardsSouth:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.row += 1;
            ball.state = BallState::EnteringFromNorth;
        }
        break;

    case BallState::ExitingTowardsWest:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.col -= 1;
            ball.state = BallState::EnteringFromEast;
        }
        break;

    }
}

void Model::progress(double milliseconds) {
    for (int r = 0; r < _rows; ++r) {
        for (int c = 0; c < _cols; ++c) {
            switch (tile(r, c).type) {
            case TileType::Rotor:
                progress_rotor(tile(r, c), milliseconds);
                break;
            default:
                break;
            }
        }
    }

    for (auto &ball : _balls) {
        // current tile
        Tile tile = _tiles[ball.col + ball.row * _cols];

        progress_ball(ball, tile, milliseconds);
    }
}
