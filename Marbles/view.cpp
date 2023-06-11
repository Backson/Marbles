#include "view.hpp"

#include "model.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void View::draw(const Model &m) const {
    ALLEGRO_COLOR line_color = al_map_rgb(127, 127, 127);

    // draw grid
    for (int r = 0; r <= m.rows(); ++r) {
        al_draw_line(_x + 0.5, _y + r * _w + 0.5, _x + 8 * _w + 0.5, _y + r * _w + 0.5, line_color, 0.0);
    }
    for (int c = 0; c <= m.cols(); ++c) {
        al_draw_line(_x + c * _w + 0.5, _y + 0.5, _x + c * _w + 0.5, _y + 5 * _w + 0.5, line_color, 0.0);
    }

    // draw tiles
    static const auto draw_track = [=](int r, int c, double x1, double y1, double x2, double y2) {
        x1 *= 0.5;
        y1 *= 0.5;
        x2 *= 0.5;
        y2 *= 0.5;

        x1 += 0.5;
        y1 += 0.5;
        x2 += 0.5;
        y2 += 0.5;

        x1 += c;
        y1 += r;
        x2 += c;
        y2 += r;

        x1 *= _w;
        y1 *= _w;
        x2 *= _w;
        y2 *= _w;

        x1 += _x;
        y1 += _y;
        x2 += _x;
        y2 += _y;

        const ALLEGRO_COLOR color = al_map_rgb(255, 255, 255);

        al_draw_line(x1, y1, x2, y2, color, 1.5);
    };

    static const auto draw_circle = [=](int row, int col, double x1, double y1, double r, ALLEGRO_COLOR color) {
        x1 *= 0.5;
        y1 *= 0.5;
        r *= 0.5;

        x1 += 0.5;
        y1 += 0.5;

        x1 += col;
        y1 += row;

        r *= _w;
        x1 *= _w;
        y1 *= _w;

        x1 += _x;
        y1 += _y;

        al_draw_filled_circle(x1, y1, r, color);
    };

    for (int r = 0; r < m.rows(); ++r) {
        for (int c = 0; c < m.cols(); ++c) {
            Tile tile = m.tile(r, c);
            switch (tile.type) {
            case TileType::CornerNorthEast:
                draw_track(r, c, 0, 0, 0, -1);
                draw_track(r, c, 0, 0, 1, 0);
                break;

            case TileType::CornerNorthWest:
                draw_track(r, c, 0, 0, 0, -1);
                draw_track(r, c, 0, 0, -1, 0);
                break;

            case TileType::CornerSouthEast:
                draw_track(r, c, 0, 0, 0, 1);
                draw_track(r, c, 0, 0, 1, 0);
                break;

            case TileType::CornerSouthWest:
                draw_track(r, c, 0, 0, 0, 1);
                draw_track(r, c, 0, 0, -1, 0);
                break;

            case TileType::Horizontal:
                draw_track(r, c, -1, 0, 1, 0);
                break;

            case TileType::Vertical:
                draw_track(r, c, 0, -1, 0, 1);
                break;

            case TileType::Crossing:
                draw_track(r, c, -1, 0, 1, 0);
                draw_track(r, c, 0, -1, 0, 1);
                break;

            case TileType::Rotor:
            {
                draw_circle(r, c, 0, 0, 0.8, al_map_rgb(0x66, 0x66, 0x66));
                double pi_half = 1.5707963267948966;
                double quarter_turns = tile.rotor.position;
                if (tile.rotor.state == RotorState::TurningClockwise) {
                    quarter_turns += tile.rotor.transition;
                }
                else if (tile.rotor.state == RotorState::TurningCounterClockwise) {
                    quarter_turns += 4;
                    quarter_turns -= tile.rotor.transition;
                }
                double angle = quarter_turns * pi_half;
                draw_circle(r, c, 0.5 * sin(angle), 0.5 * -cos(angle), 0.2, al_map_rgb(0x33, 0x33, 0x33));
                draw_circle(r, c, 0.5 * -cos(angle), 0.5 * -sin(angle), 0.2, al_map_rgb(0x33, 0x33, 0x33));
                draw_circle(r, c, 0.5 * -sin(angle), 0.5 * cos(angle), 0.2, al_map_rgb(0x33, 0x33, 0x33));
                draw_circle(r, c, 0.5 * cos(angle), 0.5 * sin(angle), 0.2, al_map_rgb(0x33, 0x33, 0x33));
                break;
            }

            default:
                break;

            }
        }
    }

    // draw balls
    for (auto &ball : m.balls()) {
        if (ball.state == BallState::None)
            continue;

        double offset_x;
        double offset_y;
        double transition = ball.transition;
        switch (ball.state) {
        case BallState::ExitingTowardsNorth:
            transition += 0.5;
        case BallState::EnteringFromSouth:
            offset_x = 0.5;
            offset_y = 1.0 - transition;
            break;

        case BallState::ExitingTowardsEast:
            transition += 0.5;
        case BallState::EnteringFromWest:
            offset_x = transition;
            offset_y = 0.5;
            break;

        case BallState::ExitingTowardsSouth:
            transition += 0.5;
        case BallState::EnteringFromNorth:
            offset_x = 0.5;
            offset_y = transition;
            break;

        case BallState::ExitingTowardsWest:
            transition += 0.5;
        case BallState::EnteringFromEast:
            offset_x = 1.0 - transition;
            offset_y = 0.5;
            break;

        default:
            throw 1;
        }

        double x = _x + _w * (ball.col + offset_x);
        double y = _y + _w * (ball.row + offset_y);

        ALLEGRO_COLOR color = al_map_rgb(0, 0, 0);

        switch (ball.type) {
        case BallType::Red:
            color = al_map_rgb(255u, 0u, 0u);
            break;
        case BallType::Green:
            color = al_map_rgb(0u, 255u, 0u);
            break;
        case BallType::Blue:
            color = al_map_rgb(0u, 0u, 255u);
            break;
        case BallType::Yellow:
            color = al_map_rgb(255u, 255u, 0u);
            break;
        default:
            throw 1;
        }

        al_draw_filled_circle(x, y, (_w / 10.0), color);
    }
}