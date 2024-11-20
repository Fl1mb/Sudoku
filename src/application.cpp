#include "application.h"

Application::Application(QWidget *parent) : QWidget(nullptr)
{
    this->setWindowTitle("Sudoku game");
    this->setFixedSize(300, 200);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    this->setPalette(palette);

    layout = std::make_unique<QVBoxLayout>();
    StartNewGameButton = std::make_unique<QPushButton>("Start Game");
    StartNewGameButton->setStyleSheet("QPushButton {"
                                      "background-color: #4CAF50;"
                                      "color: white;"
                                      "font-size: 16px;"
                                      "padding: 10px;"
                                      "border: none;"
                                      "border-radius: 5px;"
                                      "}"
                                      "QPushButton:hover {"
                                      "background-color: #45a049;"
                                      "}");

    layout->addWidget(StartNewGameButton.get());
    layout->setAlignment(Qt::AlignCenter);

    this->setLayout(layout.get());

    QObject::connect(StartNewGameButton.get(), &QPushButton::clicked, this, &Application::MakeChooser);
}

void Application::MakeChooser()
{
    this->chooser = std::make_unique<LevelChoose>();
    this->chooser->show();
    this->hide();

    QObject::connect(this->chooser.get(), &LevelChoose::sendChoice,this, &Application::HaveChosen );
}

void Application::HaveChosen(GAME_LEVEL level)
{
    QObject::disconnect(this->chooser.get(), &LevelChoose::sendChoice,this, &Application::HaveChosen );
    this->chooser->hide();
    this->chooser.reset();

    this->sudokuPtr = std::make_unique<Sudoku>(level);
    this->sudokuPtr->show();

    QObject::connect(sudokuPtr.get(), &Sudoku::GameIsOver, this, &Application::GameIsOver);
}

void Application::restart()
{
    if (this->gOv) {
        this->gOv->hide();
        this->gOv.reset();
    }
    if (this->sudokuPtr) {
        this->sudokuPtr->hide();
    }
    this->show();

}

void Application::GameIsOver(Graphics::END_OF_GAME end)
{
    if(sudokuPtr){
        QObject::disconnect(sudokuPtr.get(), &Sudoku::GameIsOver, this, &Application::GameIsOver);
    }
    this->gOv = std::make_unique<GameOver>(end);
    this->gOv->show();

    QObject::connect(gOv.get(), &GameOver::continueGame, this, &Application::restart);
}


LevelChoose::LevelChoose(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("Choose Level");
    this->setFixedSize(300, 300);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240)); // Светлый фон
    this->setPalette(palette);

    Easy = std::make_unique<QPushButton>("Easy");
    Middle = std::make_unique<QPushButton>("Middle");
    Hard = std::make_unique<QPushButton>("Hard");
    Extreeme = std::make_unique<QPushButton>("Extreeme");
    label = std::make_unique<QLabel>("Choose level :");

    label->setStyleSheet("QLabel {"
                         "font-size: 18px;"
                         "font-weight: bold;"
                         "margin-bottom: 10px;"
                         "color: black; "
                         "}");
    QString buttonStyle = "QPushButton {"
                          "background-color: #008CBA;"
                          "color: white;"
                          "font-size: 16px;"
                          "padding: 10px;"
                          "border: none;"
                          "border-radius: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: #007B9E;"
                          "}";

    Easy->setStyleSheet(buttonStyle);
    Middle->setStyleSheet(buttonStyle);
    Hard->setStyleSheet(buttonStyle);
    Extreeme->setStyleSheet(buttonStyle);

    layout = std::make_unique<QVBoxLayout>();
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    layout->addWidget(label.get());
    layout->addWidget(Easy.get());
    layout->addWidget(Middle.get());
    layout->addWidget(Hard.get());
    layout->addWidget(Extreeme.get());



    this->setLayout(layout.get());

    QObject::connect(Easy.get(), &QPushButton::clicked, this, [this](){emit this->sendChoice(GAME_LEVEL::EASY_LEVEL);});
    QObject::connect(Middle.get(), &QPushButton::clicked, this, [this](){emit this->sendChoice(GAME_LEVEL::AVERAGE_LEVEL);});
    QObject::connect(Hard.get(), &QPushButton::clicked, this, [this](){emit this->sendChoice(GAME_LEVEL::HARD_LEVEL);});
    QObject::connect(Extreeme.get(), &QPushButton::clicked, this, [this](){emit this->sendChoice(GAME_LEVEL::EXTREME_LEVEL);});


}

GameOver::GameOver(Graphics::END_OF_GAME end, QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(300, 200);

    // Установка фона
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor(240, 240, 240)); // Светлый фон
    this->setPalette(palette);

    // Создание основного layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(10); // Увеличение расстояния между элементами

    // Создание QLabel для сообщения
    QLabel *messageLabel = new QLabel(this);
    if (end == Graphics::END_OF_GAME::WIN) {
        messageLabel->setText("Congratulations! You won!");
    } else {
        messageLabel->setText("Game Over! You lost.");
    }
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #333;"); // Установка стиля
    layout->addWidget(messageLabel);

    // Создание кнопки "Continue"
    QPushButton *continueButton = new QPushButton("Continue", this);
    continueButton->setStyleSheet("QPushButton {"
                                  "background-color: #4CAF50;"
                                  "color: white;"
                                  "font-size: 16px;"
                                  "padding: 10px;"
                                  "border: none;"
                                  "border-radius: 5px;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: #45a049;"
                                  "}");
    layout->addWidget(continueButton);
    layout->setAlignment(continueButton, Qt::AlignCenter);

    // Подключение сигнала
    QObject::connect(continueButton, &QPushButton::clicked, this, &GameOver::onContinueClicked);
}

void GameOver::onContinueClicked()
{
    emit continueGame();
}
