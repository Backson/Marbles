#include "model.hpp"
#include "view.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

static constexpr double GRID_OFFSET_X = 40;
static constexpr double GRID_OFFSET_Y = 75;
static constexpr double TILE_SIZE = 90;

static constexpr int ROWS = 5;
static constexpr int COLS = 8;

void map1(Model &model) {
    model.tile(0, 0) = Tile{ TileType::CornerSouthEast };
    model.tile(1, 0) = Tile{ TileType::CornerNorthEast };
    model.tile(0, 1) = Tile{ TileType::Horizontal };
    model.tile(1, 1) = Tile{ TileType::Horizontal };
    model.tile(0, 2) = Tile{ TileType::CornerSouthWest };
    model.tile(1, 2) = Tile{ TileType::CornerNorthWest };

    model.balls().emplace_back(Ball{ BallState::ExitingTowardsEast, BallType::Green, 0, 0, 0 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsSouth, BallType::Red, 0, 0, 2 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsWest, BallType::Yellow, 0, 1, 2 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsNorth, BallType::Blue, 0, 1, 0 });

    model.tile(2, 0) = Tile{ TileType::CornerSouthEast };
    model.tile(2, 1) = Tile{ TileType::CornerSouthWest };
    model.tile(3, 0) = Tile{ TileType::Vertical };
    model.tile(3, 1) = Tile{ TileType::Vertical };
    model.tile(4, 0) = Tile{ TileType::CornerNorthEast };
    model.tile(4, 1) = Tile{ TileType::CornerNorthWest };

    model.balls().emplace_back(Ball{ BallState::ExitingTowardsSouth, BallType::Green, 0, 2, 0 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsWest, BallType::Red, 0, 2, 1 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsNorth, BallType::Yellow, 0, 4, 1 });
    model.balls().emplace_back(Ball{ BallState::ExitingTowardsEast, BallType::Blue, 0, 4, 0 });

    model.tile(2, 5) = Tile{ TileType::Crossing };
    model.tile(1, 5) = Tile{ TileType::CornerSouthEast };
    model.tile(1, 6) = Tile{ TileType::CornerSouthWest };
    model.tile(2, 6) = Tile{ TileType::CornerNorthWest };
    model.tile(2, 4) = Tile{ TileType::CornerSouthEast };
    model.tile(3, 4) = Tile{ TileType::CornerNorthEast };
    model.tile(3, 5) = Tile{ TileType::CornerNorthWest };

    model.balls().emplace_back(Ball{ BallState::ExitingTowardsSouth, BallType::Red, 0, 2, 5 });

    model.tile(1, 3) = Tile{ TileType::Rotor };
    model.tile(1, 3).rotor.state = RotorState::Resting;
    model.tile(1, 3).rotor.position = 0;

    model.tile(2, 3) = Tile{ TileType::Rotor };
    model.tile(2, 3).rotor.state = RotorState::Resting;
    model.tile(2, 3).rotor.position = 0;

    model.tile(3, 3) = Tile{ TileType::Rotor };
    model.tile(3, 3).rotor.state = RotorState::Resting;
    model.tile(3, 3).rotor.position = 0;

    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Red, 0, 1, 3, 0 });
    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Green, 0, 1, 3, 1 });
    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Blue, 0, 1, 3, 2 });
    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Yellow, 0, 1, 3, 3 });
}

void map(Model &model, const char *descr) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            char symbol = descr[c + r * COLS];
            switch (symbol) {
            case 'o':
                model.tile(r, c).type = TileType::Rotor;
                model.tile(r, c).rotor.state = RotorState::Resting;
                model.tile(r, c).rotor.position = 0;
                break;
            case '-':
                model.tile(r, c).type = TileType::Horizontal;
                break;
            case '|':
                model.tile(r, c).type = TileType::Vertical;
                break;
            default:
                model.tile(r, c).type = TileType::Empty;
                break;
            }
        }
    }
}

void map2(Model &model) {
    const char *descr =
        "o-o  o-o"
        "| |  | |"
        "o-o  o-o"
        "| |  | |"
        "o-o  o-o"
        ;
    map(model, descr);


    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Red, 0, 0, 0, 0 });
    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Green, 0, 0, 0, 1 });
    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Blue, 0, 0, 0, 2 });
    model.balls().emplace_back(Ball{ BallState::InsideRotor, BallType::Yellow, 0, 0, 0, 3 });
}

int main()
{
    al_init();
    al_install_keyboard();
    al_install_mouse();
    al_init_primitives_addon();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    ALLEGRO_DISPLAY *disp = al_create_display(800, 600);
    ALLEGRO_FONT *font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    Model model(ROWS, COLS);
    map2(model);

    View view(GRID_OFFSET_X, GRID_OFFSET_Y, TILE_SIZE);

    al_start_timer(timer);
    while (1)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            model.progress(1000.0 / 60.0);
            redraw = true;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            break;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                break;
            }
        }
        else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            int button = event.mouse.button;
            int x = event.mouse.x;
            int y = event.mouse.y;

            int col = (int)(((double)x - GRID_OFFSET_X) / TILE_SIZE);
            int row = (int)(((double)y - GRID_OFFSET_Y) / TILE_SIZE);

            if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                if (model.tile(row, col).type == TileType::Rotor) {
                    // coordinates relative to tile center, from -1 to 1
                    double rel_x = ((double)x - GRID_OFFSET_X - col * TILE_SIZE) / TILE_SIZE * 2 - 1;
                    double rel_y = ((double)y - GRID_OFFSET_Y - row * TILE_SIZE) / TILE_SIZE * 2 - 1;

                    if (rel_x * rel_x + rel_y * rel_y <= 0.3 * 0.3) {
                        if (button == 1)
                            model.turnCounterClockwise(row, col);
                        else if (button == 2)
                            model.turnClockwise(row, col);
                    }
                    else if (rel_x * rel_x + (rel_y + 0.5) * (rel_y + 0.5) <= 0.3 * 0.3) {
                        if (button == 1 || button == 2)
                            model.ejectNorth(row, col);
                    }
                    else if ((rel_x - 0.5) * (rel_x - 0.5) + rel_y * rel_y <= 0.3 * 0.3) {
                        if (button == 1 || button == 2)
                            model.ejectEast(row, col);
                    }
                    else if (rel_x * rel_x + (rel_y - 0.5) * (rel_y - 0.5) <= 0.3 * 0.3) {
                        if (button == 1 || button == 2)
                            model.ejectSouth(row, col);
                    }
                    else if ((rel_x + 0.5) * (rel_x + 0.5) + rel_y * rel_y <= 0.3 * 0.3) {
                        if (button == 1 || button == 2)
                            model.ejectWest(row, col);
                    }
                }
            }
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