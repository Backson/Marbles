#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

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
};

struct Ball {
    BallState state = BallState::None;
    BallType type;
    double transition;
    int row;
    int col;
};

enum class Tile : uint8_t {
    Empty,
    CornerNorthEast,
    CornerNorthWest,
    CornerSouthEast,
    CornerSouthWest,
    Horizontal,
    Vertical,
    Crossing,
};

class Model {
public:
    void progress(double milliseconds);

    const std::vector<Ball> &balls() const { return _balls; }
    std::vector<Ball> &balls() { return _balls; }

    const std::vector<Tile> &tiles() const { return _tiles; }
    std::vector<Tile> &tiles() { return _tiles; }


    const Tile &tile(int row, int col) const { return _tiles[col + row * 8]; }
    Tile &tile(int row, int col) { return _tiles[col + row * 8]; }



private:
    std::vector<Tile> _tiles;
    std::vector<Ball> _balls;
};

void Model::progress(double milliseconds) {
    for (auto &ball : _balls) {
        // tiles per millisecond
        double velocity = 1e-3;

        // current tile
        Tile tile = _tiles[ball.col + ball.row * 8];

        ball.transition += milliseconds * velocity;

        switch (ball.state) {
            case BallState::EnteringFromNorth:
                if (ball.transition >= 0.5) {
                    ball.transition -= 0.5;
                    switch (tile) {
                    case Tile::CornerNorthEast:
                        ball.state = BallState::ExitingTowardsEast;
                        break;
                    case Tile::CornerNorthWest:
                        ball.state = BallState::ExitingTowardsWest;
                        break;
                    case Tile::Vertical:
                    case Tile::Crossing:
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
                    switch (tile) {
                    case Tile::CornerNorthEast:
                        ball.state = BallState::ExitingTowardsNorth;
                        break;
                    case Tile::CornerSouthEast:
                        ball.state = BallState::ExitingTowardsSouth;
                        break;
                    case Tile::Horizontal:
                    case Tile::Crossing:
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
                    switch (tile) {
                    case Tile::CornerSouthEast:
                        ball.state = BallState::ExitingTowardsEast;
                        break;
                    case Tile::CornerSouthWest:
                        ball.state = BallState::ExitingTowardsWest;
                        break;
                    case Tile::Vertical:
                    case Tile::Crossing:
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
                    switch (tile) {
                    case Tile::CornerNorthWest:
                        ball.state = BallState::ExitingTowardsNorth;
                        break;
                    case Tile::CornerSouthWest:
                        ball.state = BallState::ExitingTowardsSouth;
                        break;
                    case Tile::Horizontal:
                    case Tile::Crossing:
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

class View {
public:
    View(double x, double y, double w) : _x(x), _y(y), _w(w) { }

    void draw(const Model &) const;

private:
    double _x;
    double _y;
    double _w;
};

void View::draw(const Model &m) const {
    ALLEGRO_COLOR line_color = al_map_rgb(127, 127, 127);

    // draw grid
    for (int r = 0; r <= 5; ++r) {
        al_draw_line(_x + 0.5, _y + r * _w + 0.5, _x + 8 * _w + 0.5, _y + r * _w + 0.5, line_color, 0.0);
    }
    for (int c = 0; c <= 8; ++c) {
        al_draw_line(_x + c * _w + 0.5, _y + 0.5, _x + c * _w + 0.5, _y + 5 * _w + 0.5, line_color, 0.0);
    }

    // draw tracks
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

    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 8; ++c) {
            Tile tile = m.tile(r, c);
            switch (tile) {
            case Tile::CornerNorthEast:
                draw_track(r, c, 0, 0, 0, -1);
                draw_track(r, c, 0, 0, 1, 0);
                break;

            case Tile::CornerNorthWest:
                draw_track(r, c, 0, 0, 0, -1);
                draw_track(r, c, 0, 0, -1, 0);
                break;

            case Tile::CornerSouthEast:
                draw_track(r, c, 0, 0, 0, 1);
                draw_track(r, c, 0, 0, 1, 0);
                break;

            case Tile::CornerSouthWest:
                draw_track(r, c, 0, 0, 0, 1);
                draw_track(r, c, 0, 0, -1, 0);
                break;

            case Tile::Horizontal:
                draw_track(r, c, -1, 0, 1, 0);
                break;

            case Tile::Vertical:
                draw_track(r, c, 0, -1, 0, 1);
                break;

            case Tile::Crossing:
                draw_track(r, c, -1, 0, 1, 0);
                draw_track(r, c, 0, -1, 0, 1);
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

        ALLEGRO_COLOR color = al_map_rgb(0,0,0);

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

int main()
{
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_DISPLAY *disp = al_create_display(800, 600);
    ALLEGRO_FONT *font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    Model model;

    model.tiles().resize(8 * 5);
    for (auto &tile : model.tiles()) {
        tile = Tile::Empty;
    }

    model.tile(0, 0) = Tile::CornerSouthEast;
    model.tile(1, 0) = Tile::CornerNorthEast;
    model.tile(0, 1) = Tile::Horizontal;
    model.tile(1, 1) = Tile::Horizontal;
    model.tile(0, 2) = Tile::CornerSouthWest;
    model.tile(1, 2) = Tile::CornerNorthWest;

    model.balls().emplace_back(Ball{ BallState::ExitingTowardsEast, BallType::Green, 0, 0, 0 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsSouth, BallType::Red, 0, 0, 2 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsWest, BallType::Yellow, 0, 1, 2 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsNorth, BallType::Blue, 0, 1, 0 });

    model.tile(2, 0) = Tile::CornerSouthEast;
    model.tile(2, 1) = Tile::CornerSouthWest;
    model.tile(3, 0) = Tile::Vertical;
    model.tile(3, 1) = Tile::Vertical;
    model.tile(4, 0) = Tile::CornerNorthEast;
    model.tile(4, 1) = Tile::CornerNorthWest;

    model.balls().emplace_back(Ball{ BallState::ExitingTowardsSouth, BallType::Green, 0, 2, 0 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsWest, BallType::Red, 0, 2, 1 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsNorth, BallType::Yellow, 0, 4, 1 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsEast, BallType::Blue, 0, 4, 0 });

    model.tile(2, 5) = Tile::Crossing;
    model.tile(1, 5) = Tile::CornerSouthEast;
    model.tile(1, 6) = Tile::CornerSouthWest;
    model.tile(2, 6) = Tile::CornerNorthWest;
    model.tile(2, 4) = Tile::CornerSouthEast;
    model.tile(3, 4) = Tile::CornerNorthEast;
    model.tile(3, 5) = Tile::CornerNorthWest;

    model.balls().emplace_back(Ball{ BallState::ExitingTowardsSouth, BallType::Red, 0, 2, 5 });

    View view(40, 75, 90);

    al_start_timer(timer);
    while (1)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            model.progress(1000.0 / 60.0);
            redraw = true;
        }
        else if ((event.type == ALLEGRO_EVENT_KEY_DOWN) || (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
        {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            
            view.draw(model);

            al_flip_display();

            redraw = false;
        }
    }

    al_shutdown_primitives_addon();

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}