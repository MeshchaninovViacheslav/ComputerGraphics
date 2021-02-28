#include "Player.h"
#include <iostream>
#include <chrono>
#include <thread>

bool Player::Moved() const {
    return !(coords.x == old_coords.x && coords.y == old_coords.y);
}

void Player::ProcessInput(MovementDir dir) {
    int move_dist = move_speed * 1;
    switch (dir) {
        case MovementDir::UP:
            old_coords.y = coords.y;
            coords.y += move_dist;
            break;
        case MovementDir::DOWN:
            old_coords.y = coords.y;
            coords.y -= move_dist;
            break;
        case MovementDir::LEFT:
            old_coords.x = coords.x;
            coords.x -= move_dist;
            break;
        case MovementDir::RIGHT:
            old_coords.x = coords.x;
            coords.x += move_dist;
            break;
        case MovementDir::OPEN:

        default:
            break;
    }
}

void Player::Draw(std::shared_ptr<Image> &screen, std::shared_ptr<Image> &background, int window_size) {
    if (Moved()) {
        for (int y = old_coords.y; y < old_coords.y + tileSize; ++y) {
            for (int x = old_coords.x; x < old_coords.x + tileSize; ++x) {
                screen->PutPixel(x, y, background->GetPixel(x, y));
            }
        }
        /*
        int step = move_speed;
        if (coords.x == old_coords.x && coords.y > old_coords.y) {
            for (int y = old_coords.y + step; y < old_coords.y + tileSize + step; ++y) {
                int y_black = y - tileSize * window_size / 2 - tileSize;
                int y_new = y + tileSize * window_size / 2;
                //std::cout<< tileSize * window_size / 2 << std::endl;
                for (int x = old_coords.x - tileSize * window_size / 2;
                     x < old_coords.x + tileSize + tileSize * window_size / 2; ++x) {
                    if (y_new >= 0 && y_new < screen->Height() && x >= 0 && x < screen->Width()) {
                        screen->PutPixel(x, y_new, background->GetPixel(x, y_new));
                    }
                    if (y_black >= 0 && y_black < screen->Height() && x >= 0 && x < screen->Width()) {
                        screen->PutPixel(x, y_black, backgroundColor);
                    }
                }
            }
        }
        if (coords.x == old_coords.x && coords.y < old_coords.y) {
            for (int y = std::max(old_coords.y - step, 0); y < old_coords.y + tileSize - step; ++y) {
                int y_black = y + tileSize * window_size / 2 + tileSize;
                int y_new = y - (tileSize * window_size / 2);
                for (int x = old_coords.x - tileSize * window_size / 2;
                     x < old_coords.x + tileSize + tileSize * window_size / 2; ++x) {
                    if (y_new >= 0 && y_new < screen->Height() && x >= 0 && x < screen->Width()) {
                        screen->PutPixel(x, y_new, background->GetPixel(x, y_new));
                    }
                    if (y_black >= 0 && y_black < screen->Height() && x >= 0 && x < screen->Width()) {
                        screen->PutPixel(x, y_black, backgroundColor);
                    }
                }
            }
        }
        if (coords.x > old_coords.x && coords.y == old_coords.y) {
            for (int x = old_coords.x + step; x < old_coords.x + tileSize + step; ++x) {
                int x_black = x - (tileSize * window_size / 2 + tileSize);
                int x_new = x + tileSize * window_size / 2;
                for (int y = old_coords.y - tileSize * window_size / 2;
                     y < old_coords.y + tileSize + tileSize * window_size / 2; ++y) {
                    if (x_new < screen->Width() && x_new >= 0 && y >= 0 && y < screen->Height()) {
                        screen->PutPixel(x_new, y, background->GetPixel(x_new, y));
                    }
                    if (x_black >= 0 && x_black < screen->Width() && y >= 0 && y < screen->Height()) {
                        screen->PutPixel(x_black, y, backgroundColor);
                    }
                }
            }
        }
        if (coords.x < old_coords.x && coords.y == old_coords.y) {
            for (int x = old_coords.x - step; x < old_coords.x + tileSize - step; ++x) {
                int x_black = x + tileSize * window_size / 2 + tileSize;
                int x_new = x - (tileSize * window_size / 2);
                for (int y = old_coords.y - tileSize * window_size / 2;
                     y < old_coords.y + tileSize + tileSize * window_size / 2; ++y) {
                    if (x_new < screen->Width() && x_new >= 0 && y >= 0 && y < screen->Height()) {
                        screen->PutPixel(x_new, y, background->GetPixel(x_new, y));
                    }
                    if (x_black >= 0 && x_black < screen->Width() && y >= 0 && y < screen->Height()) {
                        screen->PutPixel(x_black, y, backgroundColor);
                    }
                }
            }
        }
         */
        old_coords = coords;
    }

    for (int y = coords.y; y < coords.y + tileSize; ++y) {
        for (int x = coords.x; x < coords.x + tileSize; ++x) {
            screen->PutPixel(x, y, playerPicture.GetPixel(x - coords.x, tileSize - y + coords.y - 1));
        }
    }
    /*
    std::cout << background->HasZeroPixel() << std::endl;
    if (background->HasZeroPixel()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
     */
}