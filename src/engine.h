#ifndef ENGINE_H
#define ENGINE_H

#include <algorithm>
#include "gamefield.h"
#include <memory>
#include <random>
#include <iostream>

class Engine;
struct IMPL;



enum class GAME_LEVEL : uint8_t{
    EASY_LEVEL = 36,
    AVERAGE_LEVEL = 32,
    HARD_LEVEL =  28,
    EXTREME_LEVEL = 20
};

class Engine
{
public:
    explicit Engine(GAME_LEVEL lvl_of_game);

    void generate();
    void print();

private:
    bool isSafe(uint8_t row, uint8_t col, uint8_t val);
    bool fillSudoku();



    struct IMPL;
    std::unique_ptr<Engine::IMPL> pimpl;

    static constexpr uint8_t SIZE = 9;
    static constexpr uint8_t SUBGRID_SIZE = 3;
};

struct Engine::IMPL{
    GameField field;
    GameField answer;
    GAME_LEVEL level_of_game;
    bool isGameOver;
};

#endif // ENGINE_H
