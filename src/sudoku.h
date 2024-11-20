#ifndef SUDOKU_H
#define SUDOKU_H
#include "src/graphics.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QObject>


class Sudoku;
class Numbers;
class HelpBar;
class InfoBar;

class Sudoku : public QWidget
{
    Q_OBJECT
public:
    Sudoku(GAME_LEVEL level,  QWidget* parent = nullptr);
    ~Sudoku();

signals:
    void GameIsOver(Graphics::END_OF_GAME end);

public slots:

    void PushNumber(const QString& number);
    void EraseNumber();
    void GiveHelp();
    void CancelAction();
    void endGame(Graphics::END_OF_GAME end);

private:

    std::unique_ptr<Graphics> game;
    std::unique_ptr<QVBoxLayout> layout;
    std::unique_ptr<Numbers> numbers;
    std::unique_ptr<HelpBar> helpBar;
    std::unique_ptr<InfoBar> infoBar;

};



class Numbers : public QWidget{
    Q_OBJECT
public:
    Numbers(QWidget* parent = nullptr);
    ~Numbers();

    void SendSignal(QPushButton* button);

signals:
    void NumberPressed(const QString& number);

private:
    std::array<std::unique_ptr<QPushButton>, 9> buttons;
    std::unique_ptr<QHBoxLayout> layout;
};


class InfoBar : public QWidget{
    Q_OBJECT
public:
    InfoBar(GAME_LEVEL lvl, QWidget* parent = nullptr);
    ~InfoBar();
    void setMistakes(uint8_t mis);

public slots:
    void changeTimer();
private:
    std::unique_ptr<QHBoxLayout> layout;
    std::unique_ptr<QLabel> gameLVL;
    std::unique_ptr<QLabel> timer_l;
    std::unique_ptr<QLabel> mistakes;

    std::unique_ptr<QTimer> timer;

    int seconds;
};



class HelpBar : public QWidget{
    Q_OBJECT
public:
    explicit HelpBar( QWidget* parent = nullptr);
    ~HelpBar();

    bool isNotesMode()const;


signals:
    void NotesPressed();
    void ErasePressed();
    void getHelp();
    void cancel();

private:
    std::unique_ptr<QHBoxLayout> layout;

    std::unique_ptr<QPushButton> cancelButton;
    std::unique_ptr<QPushButton> eraseButton;
    std::unique_ptr<QPushButton> notesButton;
    std::unique_ptr<QPushButton> helpButton;


    bool NotesMode;
    uint8_t helpCount;

    static constexpr QSize Size{100,40};

};

#endif // SUDOKU_H
