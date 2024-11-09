#include "graphics.h"

Graphics::Graphics(QGraphicsView *parent): QGraphicsView(parent), scene(std::make_unique<QGraphicsScene>()),
    engine(std::make_unique<Engine>()), mistakeCounter(0)
{
    this->setScene(this->scene.get());
    this->setRenderHint(QPainter::Antialiasing);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFixedSize(SIZE);
    this->setStyleSheet("background-color: white;");

    this->NoteMode = false;

    for(uint8_t i = 0; i < 9; i++){
        for(uint8_t j =0; j< 0; j++){
            WasOrNotMistake[i][j] = false;
        }
    }
}

uint8_t Graphics::getMistakes()
{
    std::array<std::array<std::optional<uint8_t >, 9>,9> arr = this->engine->getFieldArray();

///Функция проверки на ошибки
/// Есть массив bool, типа была ли раньше такая ошибка или нет
/// если была, то она уже не считается
///

    for(uint8_t i = 0; i < 9; i++){
        for(uint8_t j = 0; j < 9; j++){
            if(this->TextArray[i][j]->toPlainText() == "" and arr[i][j] == std::nullopt)continue;
            else if(this->TextArray[i][j]->toPlainText() != "" and arr[i][j] == std::nullopt){
                if(!WasOrNotMistake[i][j])
                    mistakeCounter++;
                WasOrNotMistake[i][j] = true;
                continue;
            }
            if(this->TextArray[i][j]->toPlainText().toInt() != arr[i][j].value()){
                if(!WasOrNotMistake[i][j])
                    mistakeCounter++;
                WasOrNotMistake[i][j] = true;
            }

        }
    }

    return mistakeCounter;
}

bool Graphics::isChosen() const
{
    return this->chosenSquare.first.has_value();
}

void Graphics::changeChosen(const QString &number)
{
    ///Метод который отрисовывает цифры на поле
    /// В зависимости от режима игры
    /// Пометки или на серьезке
    ///

    if(this->NoteMode){
        NotesArray[chosenSquare.first.value()][chosenSquare.second.value()]->setPlainText(number);
    }else{
        TextArray[chosenSquare.first.value()][chosenSquare.second.value()]->setPlainText(number);
    }

    comeBack();
    chosenSquare.first = std::nullopt;
    chosenSquare.second = std::nullopt;
    HighlightedSquares.clear();
}

void Graphics::StartPlaying()
{
    ///Запускает движок генерации судоку
    /// настраивает судоку и отрисовывает

    this->engine->generate();
    this->engine->makeGameField();
    this->makeSudoku(this->engine->getGameField());
    this->update();
}

void Graphics::changeSudoku()
{
    ///метод, который отмечает поля
    /// Куда выбирается
    /// и что-то типа подсказки визуальной
    ///

    if(chosenSquare.first.has_value()){
        int x = chosenSquare.first.value();
        int y = chosenSquare.second.value();

        array[y][x]->setBrush(Qt::gray);


    }

    for(const auto& pair : HighlightedSquares){
        array[pair.second][pair.first]->setBrush(Qt::lightGray);
    }

}

void Graphics::comeBack()
{
    ///делает противоположное методу выше
    ///
    if(chosenSquare.first.has_value()){
        int x = chosenSquare.first.value();
        int y = chosenSquare.second.value();

        array[y][x]->setBrush(Qt::white);
    }

    for(const auto& pair : HighlightedSquares){
        array[pair.second][pair.first]->setBrush(Qt::white);
    }
}

void Graphics::makeSudoku(const GameField &field)
{
    ///Метод инициализирует массив 9 на 9
    /// Отрисовка, настройки шрифтов и прочая ерундистика
    ///
    ///

    QPen pen(Qt::black);
    this->scene->clear();

    for(int row =0; row < 9; row++){
        for(int col = 0; col < 9; col++){
            this->array[row][col] = std::make_unique<QGraphicsRectItem>(col *  SubSIZE.width(), row * SubSIZE.height(),SubSIZE.width(), SubSIZE.height());
            this->array[row][col]->setPen(pen);
            this->array[row][col]->setZValue(1);
            this->scene->addItem(this->array[row][col].get());

            if(field.getNumber(row, col) != 0){
                configureTextSquare(row, col, QString::number(field.getNumber(row, col)));
                this->scene->addItem(TextArray[row][col].get());
            }else{
                configureTextSquare(row, col, "");
                this->scene->addItem(TextArray[row][col].get());
            }
            NotesArray[row][col] = std::make_unique<QGraphicsTextItem>("");
            NotesArray[row][col]->setPos(row *  SubSIZE.width() + 33, col * SubSIZE.height() + 25);
            NotesArray[row][col]->setFont(QFont("Arial", 10));
            NotesArray[row][col]->setDefaultTextColor(Qt::darkGray);
            NotesArray[row][col]->setZValue(3);
            this->scene->addItem(NotesArray[row][col].get());
        }
    }
    pen.setColor(Qt::blue);
    pen.setWidth(3);
    for(int i = 0; i< 3; i++){
        for(int j = 0; j < 3; j++){
            QGraphicsRectItem* line = new QGraphicsRectItem(i *  SubSIZE.width() * 3, j * SubSIZE.height() * 3, 3 * SubSIZE.width(), 3 * SubSIZE.height());
            line->setPen(pen);
            line->setZValue(4);
            scene->addItem(line);
        }
    }
}

void Graphics::configureTextSquare(int x, int y, const QString &str)
{
    ///Вспомогательная функция
    /// Нужна чтобы в методе выше было меньше кода и более читаемо

    TextArray[x][y]  =  std::make_unique<QGraphicsTextItem>(str);
    TextArray[x][y]->setPos(x *  SubSIZE.width() + 18, y * SubSIZE.height() + 8);
    TextArray[x][y]->setFont(QFont("Arial", 16));
    TextArray[x][y]->setDefaultTextColor(Qt::black);
    TextArray[x][y]->setZValue(3);
}

bool Graphics::checkMistakes() const
{
    ///да хуй знает что это
    /// не сделал пока
    /// да наверное и не надо будет
    ///
}

void Graphics::setNodeMode(bool mode)
{
    this->NoteMode = mode;
}

void Graphics::mousePressEvent(QMouseEvent *event)
{
    ///перегруженный метод, отслеживающий нажатия мыши по View
    /// Высчитывает x и y нажатия
    /// потом считает в какой клетке это было
    /// Далее потом либо выделяет либо наоборот

    int x = event->x() / SubSIZE.width();
    int y = event->y() / SubSIZE.height();

    if(TextArray[x][y]->toPlainText() != ""){
        if(chosenSquare.first.has_value()){
            comeBack();

            int x1 = chosenSquare.first.value();
            int y1 = chosenSquare.second.value();

            chosenSquare.first = std::nullopt;
            chosenSquare.second = std::nullopt;

            if(x1 == x and y1 == y)return;
        }
        return;
    }

    if(chosenSquare.first.has_value()){
        comeBack();

        int x1 = chosenSquare.first.value();
        int y1 = chosenSquare.second.value();

        chosenSquare.first = std::nullopt;
        chosenSquare.second = std::nullopt;

        if(x1 == x and y1 == y)return;
    }

    HighlightedSquares.clear();



    chosenSquare = std::make_pair<std::optional<uint8_t>, std::optional<uint8_t>>(x, y);

    for(uint8_t i = 0; i < 9; i++){
        HighlightedSquares.push_back({x, i});
        HighlightedSquares.push_back({i, y});
    }

    int squareX = x / 3; // Индекс квадрата по горизонтали
    int squareY = y / 3; // Индекс квадрата по вертикали

    for(int i = squareX * 3; i < (squareX + 1) * 3; ++i) {
        for(int j = squareY * 3; j < (squareY + 1) * 3; ++j) {
            HighlightedSquares.push_back({i, j}); // Клетки в квадрате 3x3
        }
    }

    this->changeSudoku();
    this->update();
}
