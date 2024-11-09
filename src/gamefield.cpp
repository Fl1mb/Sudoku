#include "gamefield.h"


GameField::GameField()
{
    for(auto& arr : gameField){
        for(auto& opt : arr){
            opt = std::nullopt;
        }
    }
}

GameField::GameField(const GameField &otherField) : gameField(otherField.gameField)
{

}

GameField::GameField(GameField &&other)
{
    this->gameField = std::move(other.gameField);
}

GameField &GameField::operator=(const GameField &other)
{
    this->gameField = other.gameField;
}

GameField &GameField::operator=(GameField &&other)
{
    this->gameField = std::move(other.gameField);
}

bool GameField::operator==(const GameField &other) const
{
    return this->gameField == other.gameField;
}

bool GameField::operator !=(const GameField &other) const
{
    return this->gameField != other.gameField;
}

bool GameField::isEmpty() const
{
    return this->gameField.empty();
}

bool GameField::isEmpty(uint8_t x, uint8_t y) const
{
    return !gameField[x][y].has_value();
}

bool GameField::isValid() const
{
    for(const auto& arr : gameField){
        for(const auto& num : arr){
            if(num.value() < 0 or num.value() > 9)return false;
        }
    }
    return true;
}

void GameField::setNumber(uint8_t x, uint8_t y, uint8_t value)
{
    if(value < 0 or value > 9)return;
    gameField[x][y] = value;
}

void GameField::setEmpty(uint8_t x, uint8_t y)
{
    this->gameField[x][y] = std::nullopt;
}

uint8_t GameField::getNumber(uint8_t x, uint8_t y) const
{
    return this->isEmpty(x, y) ? 0 : this->gameField[x][y].value();
}

std::vector<std::pair<uint8_t, uint8_t> > GameField::getEmptyCoordinates() const
{
    std::vector<std::pair<uint8_t, uint8_t>> result;
    for(uint8_t i = 0; i < 10; i ++){
        for(uint8_t j = 0 ; j < 10; j ++){
            if(this->isEmpty(i,j))
                result.push_back({i, j});
        }
    }
    return result;
}

std::array<std::array<std::optional<uint8_t>, 9>, 9> GameField::getArray() const
{
    return this->gameField;
}

GameField::~GameField() = default;
