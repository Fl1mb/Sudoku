#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QObject>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QMouseEvent>
#include <array>
#include <stack>
#include "src/engine.h"

class Graphics;

class Graphics : public  QGraphicsView
{
    Q_OBJECT
public:
    Graphics(QGraphicsView* parent = nullptr);

    uint8_t getMistakes();

    bool isChosen() const;
    void changeChosen(const QString& number);
    void openRandomNumber();

    void StartPlaying();

    void eraseChosenNumber();
    void returnLastAction();

    void changeSudoku();
    void comeBack();
    void makeSudoku(const GameField& field);

    void configureTextSquare(int x, int y, const QString& str);

    bool checkMistakes() const;

    void setNodeMode(bool mode);
protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    struct Action;

    std::array<std::array<std::unique_ptr<QGraphicsRectItem>,9>,9> array;
    std::array<std::array<std::unique_ptr<QGraphicsTextItem>,9>,9> TextArray;
    std::array<std::array<std::unique_ptr<QGraphicsTextItem>,9>,9> NotesArray;

    std::vector<std::pair<uint8_t, uint8_t>> RecentlyAdded;

    std::unique_ptr<QGraphicsScene> scene;

    std::pair<std::optional<uint8_t>, std::optional<uint8_t>> chosenSquare;
    std::vector<std::pair<uint8_t, uint8_t>>HighlightedSquares;

    std::unique_ptr<Engine> engine;

    uint8_t mistakeCounter;

    std::array<std::array<bool,9>,9> WasOrNotMistake;

    std::stack<Action> ActionStack;

    bool NoteMode;

    static constexpr QSize SubSIZE{55,55};
    static constexpr QSize SIZE{500, 500};

    struct Action;
};


struct Graphics::Action{
    enum class TYPE_OF_ACTION : uint8_t{
        ERASE,
        ADD,
        MAKE_NOTE,
        DELETE_NOTE
    };

    Action(std::pair<uint8_t, uint8_t> coord_,
           TYPE_OF_ACTION act_,
           std::optional<uint8_t> numWAS, std::optional<uint8_t> numBECOME): coordinates(coord_),
        action(act_),
        numberWAS(numWAS),
        numberBECOME(numBECOME)
    {}

    std::pair<uint8_t, uint8_t> coordinates;
    std::optional<uint8_t> numberWAS;
    std::optional<uint8_t> numberBECOME;
    TYPE_OF_ACTION action;
};

#endif // GRAPHICS_H
