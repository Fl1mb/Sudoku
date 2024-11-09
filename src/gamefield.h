#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <array>
#include <cstdint>
#include <vector>
#include <optional>



class GameField
{
public:
    explicit GameField();
    explicit GameField(const GameField& otherField);
    GameField(GameField&& other);
    ~GameField();
    GameField &operator=(const GameField& other);
    GameField &operator=(GameField&& other);

    bool operator==(const GameField& other) const;
    bool operator !=(const GameField& other) const;

    bool isEmpty() const;
    bool isEmpty(uint8_t x, uint8_t y) const;
    bool isValid() const;

    void setNumber(uint8_t x, uint8_t y, uint8_t value);
    void setEmpty(uint8_t x, uint8_t y);

    [[nodiscard]] uint8_t getNumber(uint8_t x, uint8_t y) const;
    [[nodiscard]] std::vector<std::pair<uint8_t, uint8_t>> getEmptyCoordinates()const;
    [[nodiscard]] std::array<std::array<std::optional<uint8_t>, 9> ,9> getArray()const;


private:
    std::array<std::array<std::optional<uint8_t>, 9> ,9> gameField;

};

#endif // GAMEFIELD_H
