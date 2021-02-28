//
// Created by vyacheslav on 23.02.2021.
//

#include "Level.h"
#include <map>
#include <fstream>
#include <memory>
#include <iostream>
#include <optional>
#include <cmath>

std::map<char, std::vector<std::shared_ptr<Image>>> Tiles = {
        {' ', {std::make_shared<Image>("../resources/Empty.png"),
                      std::make_shared<Image>("../resources/Empty1.png"),
                      std::make_shared<Image>("../resources/Empty2.png"),
                      std::make_shared<Image>("../resources/Empty3.png"),
                      std::make_shared<Image>("../resources/Empty4.png")}},
        {'#', {std::make_shared<Image>("../resources/Wall.png"),
                      std::make_shared<Image>("../resources/Wall1.png"),
                      std::make_shared<Image>("../resources/Wall2.png"),
                      std::make_shared<Image>("../resources/Wall3.png"),
                      std::make_shared<Image>("../resources/Wall4.png"),
                      std::make_shared<Image>("../resources/Wall5.png")}},
         {'B', {std::make_shared<Image>("../resources/Bulb.png"),
                std::make_shared<Image>("../resources/Bulb1.png"),
                       std::make_shared<Image>("../resources/Bulb2.png")}}
};

Level::Level(const std::string &a_path, int numTilesInScreen, int numTilesInBackground, int windowSize) :
        background_(
                std::make_shared<Image>(numTilesInBackground * tileSize, numTilesInBackground * tileSize, numChannels)),
        screen_(std::make_shared<Image>(numTilesInScreen * tileSize, numTilesInScreen * tileSize, numChannels)),
        light_screen_(numTilesInBackground * tileSize,
                      std::vector<double>(numTilesInBackground * tileSize, 0.)),
        dataBackground_typeTile_(numTilesInBackground * tileSize,
                                 std::vector<tyleType>(numTilesInBackground * tileSize, FLOOR)),
        visible_screen_(std::make_shared<Image>(windowSize * tileSize, windowSize * tileSize, numChannels)),
        window_size(windowSize) {

    for (auto x: Tiles) {
        numsOfCurrentTiles[x.first] = 0;
        coordsOfTiles[x.first] = {};
    }

    std::map<char, std::shared_ptr<Image>> pictures = {
            {' ', std::make_shared<Image>("../resources/Empty.png")},
            {'#', std::make_shared<Image>("../resources/Wall.png")},
            {'.', std::make_shared<Image>("../resources/Floor.png")},
            {'@', std::make_shared<Image>("../resources/Hero.png")},
            {'x', std::make_shared<Image>("../resources/Exit.png")},
            {'D', std::make_shared<Image>("../resources/CloseDoor.png")},
            {'B', std::make_shared<Image>("../resources/Bulb.png")}
    };

    std::map<char, tyleType> tyleTypes = {
            {' ', EMPTY},
            {'#', WALL},
            {'.', FLOOR},
            {'@', HERO},
            {'x', EXIT},
            {'D', CLOSEDOOR},
            {'B', BULB}
    };

    std::ifstream file(a_path);

    for (int i = background_->Height() - tileSize; i >= 0; i -= tileSize) {
        for (int j = 0; j < background_->Width(); j += tileSize) {
            char tile;
            file.get(tile);
            while (tile == '\n') {
                file >> tile;
            }

            if (coordsOfTiles.find(tile) != coordsOfTiles.end()) {
                coordsOfTiles[tile].push_back({j, i});
            }

            if (tile == '@') {
                player_ = std::move(std::make_shared<Player>(Point{j, i}));
            }

            if (tile == 'B') {
                bulbsCoords.push_back({j + tileSize / 2, i + tileSize / 2});
            }

            for (int pixel_i = 0; pixel_i < tileSize; ++pixel_i) {
                for (int pixel_j = 0; pixel_j < tileSize; ++pixel_j) {
                    auto pixel = pictures[tile]->GetPixel(pixel_j, tileSize - pixel_i - 1);
                    if (tile == '@') {
                        background_->PutPixel(j + pixel_j, i + pixel_i,
                                              pictures['.']->GetPixel(pixel_j, tileSize - pixel_i - 1));
                        dataBackground_typeTile_[i + pixel_i][j + pixel_j] = tyleTypes['.'];
                    } else {
                        background_->PutPixel(j + pixel_j, i + pixel_i, pixel);
                        dataBackground_typeTile_[i + pixel_i][j + pixel_j] = tyleTypes[tile];
                    }
                    screen_->PutPixel(j + pixel_j, i + pixel_i, pixel);
                }
            }
        }
    }

    LightScreen();

    VisualizeScreen();
    /*
    for (int i = player_->GetCoords().x - window_size * tileSize / 2;
         i < player_->GetCoords().x + tileSize + window_size * tileSize / 2; ++i) {
        for (int j = player_->GetCoords().y - window_size * tileSize / 2;
             j < player_->GetCoords().y + tileSize + window_size * tileSize / 2; ++j) {
            if (i >= 0 && i < screen_->Width() && j >= 0 && j < screen_->Height()) {
                screen_->PutPixel(i, j, background_->GetPixel(i, j));
            }
        }
    }
     */
}

void Level::ProcessPlayerMovement(MovementDir direction) {
    player_->ProcessInput(direction);

    auto IsIntersect = [&](tyleType tile) {
        Point coordsPlayer = player_->GetCoords();
        int shift = 0;
        if (dataBackground_typeTile_[coordsPlayer.y + shift][coordsPlayer.x + shift] == tile ||
            dataBackground_typeTile_[coordsPlayer.y + tileSize - 1 - shift][coordsPlayer.x + shift] == tile ||
            dataBackground_typeTile_[coordsPlayer.y + shift][coordsPlayer.x + tileSize - 1 - shift] == tile ||
            dataBackground_typeTile_[coordsPlayer.y + tileSize - 1 - shift][coordsPlayer.x + tileSize - 1 - shift] ==
            tile) {
            return true;
        }
        return false;
    };

    auto MoveToBorder = [&](tyleType tile) {
        if (direction == MovementDir::UP) {
            while (IsIntersect(tile)) {
                auto coords = player_->GetCoords();
                player_->SetCoords({coords.x, coords.y - 1});
            }
        } else if (direction == MovementDir::DOWN) {
            while (IsIntersect(tile)) {
                auto coords = player_->GetCoords();
                player_->SetCoords({coords.x, coords.y + 1});
            }
        } else if (direction == MovementDir::LEFT) {
            while (IsIntersect(tile)) {
                auto coords = player_->GetCoords();
                player_->SetCoords({coords.x + 1, coords.y});
            }
        } else {
            while (IsIntersect(tile)) {
                auto coords = player_->GetCoords();
                player_->SetCoords({coords.x - 1, coords.y});
            }
        }
    };

    if (IsIntersect(WALL)) {
        MoveToBorder(WALL);
    }

    if (IsIntersect(EMPTY)) {
        playerStatus = DEAD;
        MoveToBorder(EMPTY);
    }

    if (IsIntersect(EXIT)) {
        playerStatus = WIN;
        MoveToBorder(EXIT);
    }

    if (IsIntersect(CLOSEDOOR)) {
        MoveToBorder(CLOSEDOOR);
    }
}

void Level::Draw() {
    player_->Draw(screen_, background_, this->window_size);
    if (++numUpdate == updateCycle) {
        SwitchTilesToAnimate();
        numUpdate = 0;
    }
    //LightScreen();
    VisualizeScreen();
    if (PlayerIsDead()) {
        visible_screen_->InsertImage(Image("../resources/GameOverDead2.png"));
    }
    if (PlayerIsWin()) {
        if (IsLast()) {
            visible_screen_->InsertImage(Image("../resources/GameOverWin.png"));
        } else {
            visible_screen_->InsertImage(Image("../resources/NextLevel.png"));
        }
    }
}

Pixel *Level::Data() {
    return visible_screen_->Data();
}

Level::~Level() = default;

void Level::SwitchDoor() {
    std::optional<Point> door_position = std::nullopt;
    for (int i = player_->GetCoords().y - tileSize / 2;
         i < player_->GetCoords().y + tileSize * 3 / 2; ++i) {

        int j = player_->GetCoords().x - tileSize / 2;
        if (i >= 0 && i < screen_->Height() && j >= 0 && j < screen_->Width() &&
            (dataBackground_typeTile_[i][j] == CLOSEDOOR || dataBackground_typeTile_[i][j] == OPENDOOR)) {
            door_position = {j - j % tileSize, i - i % tileSize};
        }

        j = player_->GetCoords().x + 3 * tileSize / 2;
        if (i >= 0 && i < screen_->Height() && j >= 0 && j < screen_->Width() &&
            (dataBackground_typeTile_[i][j] == CLOSEDOOR || dataBackground_typeTile_[i][j] == OPENDOOR)) {
            door_position = {j - j % tileSize, i - i % tileSize};
        }
    }
    for (int j = player_->GetCoords().x - tileSize;
         j < player_->GetCoords().x + tileSize * 2; ++j) {

        int i = player_->GetCoords().y - tileSize / 2;
        if (i >= 0 && i < screen_->Height() && j >= 0 && j < screen_->Width() &&
            (dataBackground_typeTile_[i][j] == CLOSEDOOR || dataBackground_typeTile_[i][j] == OPENDOOR)) {
            door_position = {j - j % tileSize, i - i % tileSize};
        }

        i = player_->GetCoords().y + tileSize * 3 / 2;
        if (i >= 0 && i < screen_->Height() && j >= 0 && j < screen_->Width() &&
            (dataBackground_typeTile_[i][j] == CLOSEDOOR || dataBackground_typeTile_[i][j] == OPENDOOR)) {
            door_position = {j - j % tileSize, i - i % tileSize};
        }

    }
    if (!door_position) {
        return;
    }

    Image *new_door = new Image("../resources/OpenDoor.png");
    if (dataBackground_typeTile_[door_position->y][door_position->x] == OPENDOOR) {
        delete new_door;
        new_door = new Image("../resources/CloseDoor.png");
    }


    for (int y = door_position->y; y < door_position->y + tileSize; ++y) {
        for (int x = door_position->x; x < door_position->x + tileSize; ++x) {
            if (dataBackground_typeTile_[y][x] == OPENDOOR || dataBackground_typeTile_[y][x] == CLOSEDOOR) {
                screen_->PutPixel(x, y, Fade(new_door->GetPixel(x - door_position->x, tileSize - y + door_position->y - 1), light_screen_[y][x]));
                background_->PutPixel(x, y,
                                      Fade(new_door->GetPixel(x - door_position->x, tileSize - y + door_position->y - 1), light_screen_[y][x]));
                dataBackground_typeTile_[y][x] = (dataBackground_typeTile_[y][x] == OPENDOOR) ? CLOSEDOOR : OPENDOOR;
            }
        }
    }
    //delete new_door;
}

Image Level::FadeIn(double alpha) {
    Image fadeScreen{visible_screen_->Width(), visible_screen_->Height(), 1};
    for (int y = 0; y < visible_screen_->Height(); ++y) {
        for (int x = 0; x < visible_screen_->Width(); ++x) {
            auto pixel = visible_screen_->GetPixel(x, y);
            auto fadePixel = Fade(pixel, alpha);
            //std::cout << int(fadePixel.r) << std::endl;
            fadeScreen.PutPixel(x, y, fadePixel);
        }
    }
    return fadeScreen;
}

void Level::VisualizeScreen() {
    Point coords = player_->GetCoords();
    int x_begin = coords.x - window_size * tileSize / 2;
    //std::cout << window_size << std::endl;
    //std::cout << x_begin << std::endl;
    if (x_begin < 0) {
        x_begin = 0;
    } else if (x_begin + window_size * tileSize >= screen_->Width()) {
        x_begin = screen_->Width() - window_size * tileSize;
    }
    //std::cout << x_begin << std::endl;
    int y_begin = coords.y - window_size * tileSize / 2;
    if (y_begin < 0) {
        y_begin = 0;
    } else if (y_begin + window_size * tileSize >= screen_->Height()) {
        y_begin = screen_->Height() - window_size * tileSize;
    }
    //std::cout << x_begin << " " << y_begin << std::endl;
    for (int i = 0; i < visible_screen_->Height(); ++i) {
        for (int j = 0; j < visible_screen_->Width(); ++j) {
            int x = x_begin + j, y = y_begin + i;
            visible_screen_->PutPixel(j, i, screen_->GetPixel(x, y));
        }
    }
}

void Level::SwitchTilesToAnimate() {
    for (auto &tile: coordsOfTiles) {
        char signTile = tile.first;
        //std::cout << numsOfCurrentTiles[signTile] << std::endl;
        for (auto coords: tile.second) {
            auto newAnimation = Tiles[signTile][numsOfCurrentTiles[signTile]];
            //std::cout << "NEW" << std::endl;
            for (int y = coords.y; y < coords.y + tileSize; ++y) {
                for (int x = coords.x; x < coords.x + tileSize; ++x) {
                    double alpha = light_screen_[y][x];
                    screen_->PutPixel(x, y, Fade(newAnimation->GetPixel(x - coords.x, tileSize - y + coords.y - 1), alpha));
                }
            }
        }
        ++numsOfCurrentTiles[signTile];
        if (numsOfCurrentTiles[signTile] == Tiles[signTile].size()) {
            numsOfCurrentTiles[signTile] = 0;
        }
    }
}

void Level::LightScreen() {
    // model: alpha = 1 / (AR) + B, B=0.2, A=0.125
    for (int x = 0; x < screen_->Width(); ++x) {
        for (int y = 0; y < screen_->Width(); ++y) {
            double alpha = minLightAlpha;
            for (const auto &bulb_coords: bulbsCoords) {
                double R = std::sqrt(
                        (x - bulb_coords.x) * (x - bulb_coords.x) + (y - bulb_coords.y) * (y - bulb_coords.y));
                alpha += GetAlphaLight(R) - minLightAlpha;
            }
            alpha = std::min(alpha, 1.);
            //std::cout << alpha << std::endl;
            background_->PutPixel(x, y, Fade(background_->GetPixel(x, y), alpha));
            screen_->PutPixel(x, y, Fade(screen_->GetPixel(x, y), alpha));
            light_screen_[y][x] = alpha;
        }
    }
}

double Level::GetAlphaLight(double R) const {
    static int minR = 10, maxR = 300;
    static double alphaMin = minLightAlpha;
    static double A = 1. / (maxR - minR) * (1 / alphaMin - 1);
    static double B = 1 - A * minR;

    if (R < minR) {
        return 1.;
    }
    if (R > maxR) {
        return alphaMin;
    }
    return 1 / (A * R + B);

}