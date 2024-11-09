#include "sudoku.h"

Sudoku::Sudoku(QWidget *parent) : QWidget(parent),
    game(std::make_unique<Graphics>()),
    numbers(std::make_unique<Numbers>()),
    layout(std::make_unique<QVBoxLayout>()),
    helpBar(std::make_unique<HelpBar>()),
    infoBar(std::make_unique<InfoBar>())
{
    layout->addWidget(infoBar.get());
    layout->addWidget(game.get());
    layout->addWidget(helpBar.get());
    layout->addWidget(numbers.get());

    QObject::connect(numbers.get(), Numbers::NumberPressed, this, &Sudoku::PushNumber);
    QObject::connect(helpBar.get(), &HelpBar::NotesPressed, this, [this](){
        this->game->setNodeMode(this->helpBar->isNotesMode());
    });

    this->setLayout(layout.get());
    this->move(500,0);

    this->game->StartPlaying();
}


void Sudoku::PushNumber(const QString &number)
{
    if(game->isChosen()){
        this->game->changeChosen(number);
        this->infoBar->setMistakes(this->game->getMistakes());

    }
}

Numbers::Numbers(QWidget *parent) : QWidget(parent),
    layout(std::make_unique<QHBoxLayout>())
{
    for(uint8_t i = 0; i < 9;i ++){
        buttons[i] = std::make_unique<QPushButton>(QString::number(i + 1));
        buttons[i]->setFixedSize(QSize(40,40));
        layout->addWidget(buttons[i].get());

        QObject::connect(buttons[i].get(), &QPushButton::clicked, this, [this, button = buttons[i].get()](){this->SendSignal(button);});
    }

    layout->setSpacing(1);
    this->setLayout(layout.get());

}

Numbers::~Numbers()= default;

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
    NotesMode(false)
{
    cancelButton->setText("cancel");
    eraseButton->setText("erase");
    helpButton->setText("help");
    notesButton->setText("make notes");

    cancelButton->setFixedSize(Size);
    eraseButton->setFixedSize(Size);
    helpButton->setFixedSize(Size);
    notesButton->setFixedSize(Size);

    QObject::connect(notesButton.get(), &QPushButton::clicked, this, [this, button = notesButton.get()](){
        this->NotesMode = !this->NotesMode;
        if(NotesMode)
            this->notesButton->setText("Notes On");
        else
            this->notesButton->setText("Make note");

    });

    QObject::connect(notesButton.get(), &QPushButton::clicked, this, [this](){emit this->NotesPressed();});

    layout->addWidget(cancelButton.get());
    layout->addWidget(eraseButton.get());
    layout->addWidget(notesButton.get());
    layout->addWidget(helpButton.get());

    this->setLayout(layout.get());
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
