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

void Model::eject(int row, int col, int direction) {
    if (tile(row, col).type == TileType::Rotor && tile(row, col).rotor.state == RotorState::Resting && tile(row, col).rotor.connected[direction]) {
        int position = (direction - tile(row, col).rotor.position + 4) % 4;

        for (auto &ball : _balls) {
            if (ball.row == row && ball.col == col && ball.state == BallState::InsideRotor && ball.rotor_position == position) {
                
                static constexpr BallState exiting_states[4] = {
                    BallState::ExitingTowardsNorth,
                    BallState::ExitingTowardsEast,
                    BallState::ExitingTowardsSouth,
                    BallState::ExitingTowardsWest,
                };
                
                ball.state = exiting_states[direction];
                ball.transition = 0.25;
            }
        }
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
            tile.rotor.position += 3;
            tile.rotor.position %= 4;
        }
    }
}

static void progress_ball(Ball &ball, Tile &tile, double milliseconds) {
    // tiles per millisecond
    double velocity = 1e-3;
    ball.transition += milliseconds * velocity;

    // exiting balls don't care about the tile type
    switch (ball.state) {
    case BallState::ExitingTowardsSouth:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.state = BallState::EnteringFromNorth;
            ++ball.row;
        }
        break;

    case BallState::ExitingTowardsWest:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.state = BallState::EnteringFromEast;
            --ball.col;
        }
        break;

    case BallState::ExitingTowardsNorth:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.state = BallState::EnteringFromSouth;
            --ball.row;
        }
        break;

    case BallState::ExitingTowardsEast:
        if (ball.transition >= 0.5) {
            ball.transition -= 0.5;
            ball.state = BallState::EnteringFromWest;
            ++ball.col;
        }
        break;
    }

    switch (tile.type) {
        case TileType::Rotor:
            if (ball.transition >= 0.25) {
                int direction;
                switch (ball.state) {
                case BallState::EnteringFromNorth: direction = 0; break;
                case BallState::EnteringFromEast: direction = 1; break;
                case BallState::EnteringFromSouth: direction = 2; break;
                case BallState::EnteringFromWest: direction = 3; break;
                default: direction = -1; break;
                }
                if (direction >= 0) {
                    int position = (direction - tile.rotor.position + 4) % 4;

                    if (tile.rotor.state == RotorState::Resting && !tile.rotor.taken[position]) {
                        tile.rotor.taken[position] = true;
                        ball.state = BallState::InsideRotor;
                        ball.rotor_position = position;
                        ball.transition = 0;
                    }
                    else {
                        switch (ball.state) {
                        case BallState::EnteringFromNorth:
                            ball.state = BallState::ExitingTowardsNorth;
                            break;
                        case BallState::EnteringFromEast:
                            ball.state = BallState::ExitingTowardsEast;
                            break;
                        case BallState::EnteringFromSouth:
                            ball.state = BallState::ExitingTowardsSouth;
                            break;
                        case BallState::EnteringFromWest:
                            ball.state = BallState::ExitingTowardsWest;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            break;

        case TileType::CornerNorthEast:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromNorth:
                    ball.state = BallState::ExitingTowardsEast;
                    break;
                case BallState::EnteringFromEast:
                    ball.state = BallState::ExitingTowardsNorth;
                    break;
                default:
                    break;
                }
            }
            break;

        case TileType::CornerNorthWest:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromNorth:
                    ball.state = BallState::ExitingTowardsWest;
                    break;
                case BallState::EnteringFromWest:
                    ball.state = BallState::ExitingTowardsNorth;
                    break;
                default:
                    break;
                }
            }
            break;

        case TileType::CornerSouthEast:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromSouth:
                    ball.state = BallState::ExitingTowardsEast;
                    break;
                case BallState::EnteringFromEast:
                    ball.state = BallState::ExitingTowardsSouth;
                    break;
                default:
                    break;
                }
            }
            break;

        case TileType::CornerSouthWest:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromSouth:
                    ball.state = BallState::ExitingTowardsWest;
                    break;
                case BallState::EnteringFromWest:
                    ball.state = BallState::ExitingTowardsSouth;
                    break;
                default:
                    break;
                }
            }
            break;

        case TileType::Horizontal:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromEast:
                    ball.state = BallState::ExitingTowardsWest;
                    break;
                case BallState::EnteringFromWest:
                    ball.state = BallState::ExitingTowardsEast;
                    break;
                default:
                    break;
                }
            }
            break;

        case TileType::Vertical:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromNorth:
                    ball.state = BallState::ExitingTowardsSouth;
                    break;
                case BallState::EnteringFromSouth:
                    ball.state = BallState::ExitingTowardsNorth;
                    break;
                default:
                    break;
                }
            }
            break;

        case TileType::Crossing:
            if (ball.transition >= 0.5) {
                ball.transition -= 0.5;
                switch (ball.state) {
                case BallState::EnteringFromNorth:
                    ball.state = BallState::ExitingTowardsSouth;
                    break;
                case BallState::EnteringFromEast:
                    ball.state = BallState::ExitingTowardsWest;
                    break;
                case BallState::EnteringFromSouth:
                    ball.state = BallState::ExitingTowardsNorth;
                    break;
                case BallState::EnteringFromWest:
                    ball.state = BallState::ExitingTowardsEast;
                    break;
                default:
                    break;
                }
            }
            break;

        default:
            break;
    }
}

static void recompute_taken(Model &model) {
    // set all taken values of all rotors to false
    for (int r = 0; r < model.rows(); ++r) {
        for (int c = 0; c < model.cols(); ++c) {
            Tile &tile = model.tile(r, c);
            if (tile.type == TileType::Rotor) {
                for (int i = 0; i < 4; ++i) {
                    tile.rotor.taken[i] = false;
                }
            }
        }
    }

    // if a ball is in the InsideRotor state, set the taken value to true
    for (auto &ball : model.balls()) {
        if (ball.state == BallState::InsideRotor) {
            Tile &tile = model.tile(ball.row, ball.col);
            if (tile.type == TileType::Rotor) {
                tile.rotor.taken[ball.rotor_position] = true;
            }
        }
    }
}


static void recompute_connected(Model &model) {
    for (int r = 0; r < model.rows(); ++r) {
        for (int c = 0; c < model.cols(); ++c) {
            Tile &tile = model.tile(r, c);
            if (tile.type == TileType::Rotor) {
                // north
                tile.rotor.connected[0] = r > 0 && model.tile(r - 1, c).type != TileType::Empty;

                // east
                tile.rotor.connected[1] = c < model.cols() - 1 && model.tile(r, c + 1).type != TileType::Empty;

                // south
                tile.rotor.connected[2] = r < model.rows() - 1 && model.tile(r + 1, c).type != TileType::Empty;

                // west
                tile.rotor.connected[3] = c > 0 && model.tile(r, c - 1).type != TileType::Empty;
            }
        }
    }
}

void Model::progress(double milliseconds) {
    // TODO get rid of this inefficient nonsense
    recompute_taken(*this);
    recompute_connected(*this);

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
