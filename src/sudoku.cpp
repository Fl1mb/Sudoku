#include "sudoku.h"

Sudoku::Sudoku(GAME_LEVEL level , QWidget *parent) : QWidget(parent),
    game(std::make_unique<Graphics>(level)),
    numbers(std::make_unique<Numbers>()),
    layout(std::make_unique<QVBoxLayout>()),
    helpBar(std::make_unique<HelpBar>()),
    infoBar(std::make_unique<InfoBar>(level))
{
    layout->addWidget(infoBar.get());
    layout->addWidget(game.get());
    layout->addWidget(helpBar.get());
    layout->addWidget(numbers.get());

    QObject::connect(numbers.get(), Numbers::NumberPressed, this, &Sudoku::PushNumber);
    QObject::connect(helpBar.get(), &HelpBar::NotesPressed, this, [this](){
        this->game->setNodeMode(this->helpBar->isNotesMode());
    });
    QObject::connect(helpBar.get(), &HelpBar::getHelp, this, &Sudoku::GiveHelp);
    QObject::connect(helpBar.get(), &HelpBar::ErasePressed, this, &Sudoku::EraseNumber);
    QObject::connect(helpBar.get(), &HelpBar::cancel, this, &Sudoku::CancelAction);
    QObject::connect(game.get(), &Graphics::GameEnd, this, &Sudoku::endGame);

    this->setLayout(layout.get());
    this->move(500,0);
    this->setStyleSheet("background-color: #CFD5D8");
    this->setWindowIcon(QIcon(":/ico/icon.png"));

    this->game->StartPlaying();
}

Sudoku::~Sudoku()
{
    QObject::disconnect(numbers.get(), Numbers::NumberPressed, this, &Sudoku::PushNumber);
    QObject::disconnect(helpBar.get(), &HelpBar::getHelp, this, &Sudoku::GiveHelp);
    QObject::disconnect(helpBar.get(), &HelpBar::ErasePressed, this, &Sudoku::EraseNumber);
    QObject::disconnect(helpBar.get(), &HelpBar::cancel, this, &Sudoku::CancelAction);
    QObject::disconnect(game.get(), &Graphics::GameEnd, this, &Sudoku::endGame);
    QObject::disconnect(helpBar.get(), &HelpBar::NotesPressed, this, nullptr);
}



void Sudoku::PushNumber(const QString &number)
{
    if(game and game->isChosen()){
        this->game->changeChosen(number);
        this->infoBar->setMistakes(this->game->getMistakes());

    }
}

void Sudoku::EraseNumber()
{
    if(game->isChosen()){
        this->game->eraseChosenNumber();
    }
}

void Sudoku::GiveHelp()
{
    this->game->openRandomNumber();
}

void Sudoku::CancelAction()
{
    this->game->returnLastAction();
}

void Sudoku::endGame(Graphics::END_OF_GAME end)
{
    emit this->GameIsOver(end);
}

Numbers::Numbers(QWidget *parent) : QWidget(parent),
    layout(std::make_unique<QHBoxLayout>())
{
    QString style{("QPushButton{"
                  "font: bold 14px;"
                  "color: black;"
                  "background-color: #A4B0B7;"
                  "padding: 10px;"
                  "border-radius: 10px;"
                  "}")};

    for(uint8_t i = 0; i < 9;i ++){
        buttons[i] = std::make_unique<QPushButton>(QString::number(i + 1));
        buttons[i]->setFixedSize(QSize(40,40));
        buttons[i]->setStyleSheet(style);

        layout->addWidget(buttons[i].get());

        QObject::connect(buttons[i].get(), &QPushButton::clicked, this, [this, button = buttons[i].get()](){this->SendSignal(button);});

    }

    layout->setSpacing(1);
    this->setLayout(layout.get());

}

Numbers::~Numbers(){
    for(uint8_t i = 0; i < 9;i ++){
        QObject::disconnect(buttons[i].get(), &QPushButton::clicked, this, nullptr);
    }
}

void Numbers::SendSignal(QPushButton *button)
{
    emit NumberPressed(button->text());
}

HelpBar::HelpBar(QWidget *parent) : QWidget(parent),
    layout(std::make_unique<QHBoxLayout>()),
    cancelButton(std::make_unique<QPushButton>()),
    eraseButton(std::make_unique<QPushButton>()),
    helpButton(std::make_unique<QPushButton>()),
    notesButton(std::make_unique<QPushButton>()),
    NotesMode(false),
    helpCount(3)
{
    cancelButton->setText("cancel");
    eraseButton->setText("erase");
    helpButton->setText("help " + QString::number(helpCount) + "/3");
    notesButton->setText("make notes");

    cancelButton->setFixedSize(Size);
    eraseButton->setFixedSize(Size);
    helpButton->setFixedSize(Size);
    notesButton->setFixedSize(Size);

    QString style{("QPushButton{"
                  "font: bold 14px;"
                  "color: black;"
                  "background-color: #A4B0B7;"
                  "padding: 10px;"
                  "border-radius: 10px;"
                  "}")};

    cancelButton->setStyleSheet(style);
    eraseButton->setStyleSheet(style);
    helpButton->setStyleSheet(style);
    notesButton->setStyleSheet(style);

    QObject::connect(notesButton.get(), &QPushButton::clicked, this, [this, button = notesButton.get()](){
        this->NotesMode = !this->NotesMode;
        if(NotesMode)
            this->notesButton->setText("Notes On");
        else
            this->notesButton->setText("make note");

    });

    QObject::connect(notesButton.get(), &QPushButton::clicked, this, [this](){emit this->NotesPressed();});
    QObject::connect(eraseButton.get(), &QPushButton::clicked, this, [this](){emit this->ErasePressed();});
    QObject::connect(helpButton.get(), &QPushButton::clicked, this, [this](){
        if(this->helpCount > 0){
            emit this->getHelp();
            this->helpButton->setText("help " + QString::number(--helpCount)  + "/3");
        }else
            this->helpButton->setText("Can't help!");
    });
    QObject::connect(cancelButton.get(), &QPushButton::clicked, this , [this](){emit this->cancel();});

    layout->addWidget(cancelButton.get());
    layout->addWidget(eraseButton.get());
    layout->addWidget(notesButton.get());
    layout->addWidget(helpButton.get());

    this->setLayout(layout.get());
}

HelpBar::~HelpBar()
{
    QObject::disconnect(notesButton.get(), &QPushButton::clicked, this, nullptr);
    QObject::disconnect(eraseButton.get(), &QPushButton::clicked, this, nullptr);
    QObject::disconnect(helpButton.get(), &QPushButton::clicked, this, nullptr);
    QObject::disconnect(cancelButton.get(), &QPushButton::clicked, this , nullptr);
}

bool HelpBar::isNotesMode() const
{
    return this->NotesMode;
}




InfoBar::InfoBar(GAME_LEVEL lvl, QWidget *parent): QWidget(parent),
    layout(std::make_unique<QHBoxLayout>()),
    seconds(0)
{
    switch(lvl){
    case GAME_LEVEL::EASY_LEVEL:
        gameLVL = std::make_unique<QLabel>("Level: Easy");
        break;
    case GAME_LEVEL::AVERAGE_LEVEL:
        gameLVL = std::make_unique<QLabel>("Level: Medium");
        break;
    case GAME_LEVEL::EXTREME_LEVEL:
        gameLVL = std::make_unique<QLabel>("Level: Extreme");
        break;
    case GAME_LEVEL::HARD_LEVEL:
        gameLVL = std::make_unique<QLabel>("Level: Hard");
        break;
    }





    timer = std::make_unique<QTimer>();
    mistakes = std::make_unique<QLabel>("0/3");
    timer_l= std::make_unique<QLabel>("00:00");


    layout->addWidget(gameLVL.get());
    layout->addWidget(timer_l.get());
    layout->addWidget(mistakes.get());

    QString labelStyle = "QLabel {"
                         "font: bold 14px;" // Установка жирного шрифта
            "color: white;"     // Цвет текста
            "padding: 5px;"     // Отступы
            "background-color: rgba(0, 0, 0, 0.5);" // Полупрозрачный черный фон
            "border-radius: 5px;" // Закругленные углы
            "}";

    gameLVL->setStyleSheet(labelStyle);
    mistakes->setStyleSheet(labelStyle);
    timer_l->setStyleSheet(labelStyle);

    QObject::connect(timer.get(), &QTimer::timeout, this, &InfoBar::changeTimer);
    timer->start(1000);

    this->setLayout(layout.get());
}

InfoBar::~InfoBar()
{
    QObject::disconnect(timer.get(), &QTimer::timeout, this, &InfoBar::changeTimer);
}

void InfoBar::setMistakes(uint8_t mis)
{
    this->mistakes->setText(QString::number(mis) + "/" + "3");
}


void InfoBar::changeTimer()
{
    seconds++;

    QString minute = QString::number(seconds / 60);
    QString second = QString::number(seconds % 60);

    if(minute.size() == 1)minute = "0" + minute;
    if(second.size() == 1)second = "0" + second;

    timer_l->setText(minute + ":" + second);
}
