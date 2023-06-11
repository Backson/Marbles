#include "model.hpp"

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

void Model::progress(double milliseconds) {

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            switch (tile(r, c).type) {
            case TileType::Rotor:
                if (tile(r, c).rotor.state == RotorState::TurningClockwise)
                {
                    tile(r, c).rotor.transition += milliseconds / 1000.0 * 60.0 / 5.0;
                    if (tile(r, c).rotor.transition >= 1.0) {
                        tile(r, c).rotor.state = RotorState::Resting;
                        tile(r, c).rotor.position += 1;
                        tile(r, c).rotor.position %= 4;
                    }
                }
                else if (tile(r, c).rotor.state == RotorState::TurningCounterClockwise)
                {
                    tile(r, c).rotor.transition += milliseconds / 1000.0 * 60.0 / 5.0;
                    if (tile(r, c).rotor.transition >= 1.0) {
                        tile(r, c).rotor.state = RotorState::Resting;
                        tile(r, c).rotor.position += 4;
                        tile(r, c).rotor.position -= 1;
                        tile(r, c).rotor.position %= 4;
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    for (auto &ball : _balls) {
        // tiles per millisecond
        double velocity = 1e-3;

        // current tile
        Tile tile = _tiles[ball.col + ball.row * COLS];

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
}
