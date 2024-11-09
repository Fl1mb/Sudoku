#ifndef ENGINE_H
#define ENGINE_H

#include <algorithm>
#include "gamefield.h"
#include <memory>
#include <random>
#include <iostream>
#include <unordered_set>

class Engine;
struct pair_hash;


enum class GAME_LEVEL : uint8_t{
    EASY_LEVEL = 36,
    AVERAGE_LEVEL = 32,
    HARD_LEVEL =  28,
    EXTREME_LEVEL = 20
};

class Engine
{
public:
    explicit Engine(GAME_LEVEL lvl_of_game = GAME_LEVEL::EASY_LEVEL);


    void generate();
    void clear();
    void print();
    void makeGameField();

    std::array<std::array<std::optional<uint8_t >, 9>,9> getFieldArray()const;
    GameField& getGameField()const;

private:
    bool isSafe(uint8_t row, uint8_t col, uint8_t val);
    bool fillSudoku();
    bool isValid();

    std::unordered_set<std::pair<int, int>, pair_hash> getRandomPairs();

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

struct pair_hash{
    template <class T1, class T2>
        std::size_t operator () (const std::pair<T1, T2>& pair) const {
            auto hash1 = std::hash<T1>{}(pair.first);
            auto hash2 = std::hash<T2>{}(pair.second);
            return hash1 ^ hash2;
        }
};

#endif // ENGINE_H
