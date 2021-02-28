//
// Created by vyacheslav on 23.02.2021.
//

#ifndef MAIN_LEVEL_H
#define MAIN_LEVEL_H

#include "Image.h"
#include "Player.h"

#include<vector>
#include <memory>
#include <map>

enum tyleType {
    FLOOR, WALL, EMPTY, HERO, EXIT, CLOSEDOOR, OPENDOOR, BULB
};

enum PlayerStatus {
    ALIVE, WIN, DEAD
};

class Level {
public:
    Level(const std::string &a_path, int numTilesInScreen, int numTilesInBackground, int windowSize);

    void ProcessPlayerMovement(MovementDir direction);

    void Draw();

    Pixel *Data();

    ~Level();

    bool PlayerIsDead() {
        return playerStatus == DEAD;
    }

    bool PlayerIsWin() {
        return playerStatus == WIN;
    }

    void SwitchDoor();

    void MakeLast() {
        lastLevel = true;
    }

    bool IsLast() const {
        return lastLevel;
    }

    Image FadeIn(double alpha);

    void VisualizeScreen();

    void SwitchTilesToAnimate();

    void LightScreen();

    void LightAnimations();

private:
    std::shared_ptr<Image> background_;
    std::shared_ptr<Image> screen_;
    std::shared_ptr<Image> visible_screen_;
    std::vector<std::vector<double>> light_screen_;
    std::shared_ptr<Player> player_;
    std::vector<std::vector<tyleType>> dataBackground_typeTile_;
    int window_size;
    std::map<char, int> numsOfCurrentTiles;
    std::map<char, std::vector<Point>> coordsOfTiles;
    int numUpdate = 0;

    std::vector<Point> bulbsCoords;


private:
    PlayerStatus playerStatus = ALIVE;
    bool lastLevel = false;
    int updateCycle = 20;
    double minLightAlpha = 0.5;

private:
    double GetAlphaLight(double R) const;
};

#endif //MAIN_LEVEL_H
