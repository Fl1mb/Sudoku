#ifndef APPLICATION_H
#define APPLICATION_H

#include "sudoku.h"
#include <QLabel>

class GameOver;
class Application;
class LevelChoose;


class Application: public QWidget
{
    Q_OBJECT
public:
    Application(QWidget* parent = nullptr);

public slots:
    void MakeChooser();
    void HaveChosen(GAME_LEVEL level);
    void restart();
    void GameIsOver(Graphics::END_OF_GAME end);

private:
    std::unique_ptr<Sudoku> sudokuPtr;
    std::unique_ptr<QPushButton> StartNewGameButton;
    std::unique_ptr<QVBoxLayout> layout;
    std::unique_ptr<LevelChoose> chooser;
    std::unique_ptr<GameOver> gOv;
};

class LevelChoose : public QWidget{
    Q_OBJECT
public:
    LevelChoose(QWidget* parent = nullptr);

signals:
    void sendChoice(GAME_LEVEL level);

private:
    std::unique_ptr<QPushButton> Easy;
    std::unique_ptr<QPushButton> Middle;
    std::unique_ptr<QPushButton> Hard;
    std::unique_ptr<QPushButton> Extreeme;
    std::unique_ptr<QLabel> label;
    std::unique_ptr<QVBoxLayout> layout;

};

class GameOver : public QWidget{
    Q_OBJECT
public:
    GameOver(Graphics::END_OF_GAME end, QWidget* parent = nullptr);

signals:
    void continueGame();
private slots:
    void onContinueClicked();

};

#endif // APPLICATION_H
