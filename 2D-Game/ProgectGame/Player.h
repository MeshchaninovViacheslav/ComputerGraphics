#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <memory>

struct Point {
    int x;
    int y;
};

enum class MovementDir {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    OPEN
};

struct Player {
    explicit Player(Point pos = {.x = 10, .y = 10}) :
            coords(pos), old_coords(coords),  playerPicture("../resources/Hero.png")
            {};

    bool Moved() const;

    void ProcessInput(MovementDir dir);

    void Draw(std::shared_ptr<Image> &screen, std::shared_ptr<Image>& background, int window_size);

    Point GetCoords () const {
        return coords;
    }

    void SetCoords(Point pos) {
        coords = pos;
    }

private:
    Point coords{.x = 10, .y = 10};
    Point old_coords{.x = 10, .y = 10};
    Pixel color{.r = 255, .g = 255, .b = 0, .a = 255};
    int move_speed = 4;
    Image playerPicture;
};

#endif //MAIN_PLAYER_H
