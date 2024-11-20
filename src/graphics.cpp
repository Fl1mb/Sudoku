#include "graphics.h"

Graphics::Graphics(GAME_LEVEL lvl, QGraphicsView *parent): QGraphicsView(parent), scene(std::make_unique<QGraphicsScene>()),
    engine(std::make_unique<Engine>(lvl)), mistakeCounter(0), ActionStack(std::stack<Action>())
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

Graphics::~Graphics()
{
    for (auto& row : array) {
        for (auto& item : row) {
            if (item) {
                scene->removeItem(item.get()); // Удаляем элемент из сцены
                item.reset();
            }
        }
    }

    for (auto& row : TextArray) {
        for (auto& item : row) {
            if (item) {
                scene->removeItem(item.get()); // Удаляем элемент из сцены
                item.reset();
            }
        }
    }

    for (auto& row : NotesArray) {
        for (auto& item : row) {
            if (item) {
                scene->removeItem(item.get()); // Удаляем элемент из сцены
                item.reset(); // Обнуляем указатель
            }
        }
    }

    // Удаляем сцену, если она еще не была удалена
    scene.reset();
}

uint8_t Graphics::getMistakes()
{
    ///Функция проверки на ошибки
    /// Есть массив bool, типа была ли раньше такая ошибка или нет
    /// если была, то она уже не считается
    for(uint8_t i = 0; i < 9; i++){
        for(uint8_t j = 0; j < 9; j++){
            if(this->TextArray[i][j]->toPlainText() == "")continue;
            if(this->TextArray[i][j]->toPlainText().toInt() != this->engine->getAnswer(i, j)){
                if(!WasOrNotMistake[i][j])
                    mistakeCounter++;
                WasOrNotMistake[i][j] = true;
            }

        }
    }

    if(mistakeCounter > 3)emit this->GameEnd(Graphics::END_OF_GAME::LOSE);
    return mistakeCounter;
}

uint8_t Graphics::getEmptyFields() const
{
    uint8_t result{0};
    for(uint8_t i = 0; i <9; i++){
        for(uint8_t j = 0; j < 9; j++){
            if(TextArray[i][j]->toPlainText() == "")result++;
        }
    }
    return result;
}

bool Graphics::isChosen() const
{
    return this->chosenSquare.first.has_value();
}

bool Graphics::isWin() const
{
    for(uint8_t i = 0; i <9; i++){
        for(uint8_t j = 0; j < 9; j++){
            if(TextArray[i][j]->toPlainText().toUInt() != this->engine->getAnswer(i,j))return false;
        }
    }
    return true;
}

void Graphics::changeChosen(const QString &number)
{
    ///Метод который отрисовывает цифры на поле
    /// В зависимости от режима игры
    /// Пометки или на серьезке

    if(this->NoteMode){
        NotesArray[chosenSquare.first.value()][chosenSquare.second.value()]->setPlainText(number);
        ActionStack.push({{chosenSquare.first.value(), chosenSquare.second.value()},
                          Graphics::Action::TYPE_OF_ACTION::MAKE_NOTE,
                          std::nullopt ,
                          static_cast<uint8_t>(number.toInt())});
    }else{
        TextArray[chosenSquare.first.value()][chosenSquare.second.value()]->setPlainText(number);
        ActionStack.push({{chosenSquare.first.value(), chosenSquare.second.value()},
                          Graphics::Action::TYPE_OF_ACTION::ADD,
                          std::nullopt,
                          static_cast<uint8_t>(number.toInt())});
    }
    RecentlyAdded.push_back({chosenSquare.first.value(), chosenSquare.second.value()});


    if(!this->getEmptyFields()){
        if(this->isWin()){
            emit this->GameEnd(Graphics::END_OF_GAME::WIN);
        }
    }
    comeBack();
    chosenSquare.first = std::nullopt;
    chosenSquare.second = std::nullopt;
    HighlightedSquares.clear();
}

void Graphics::openRandomNumber()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>dis(0,9);

    uint8_t x = dis(gen), y = dis(gen);
    while(this->TextArray[x][y]->toPlainText() != ""){
        x = dis(gen);
        y = dis(gen);
    }

    this->TextArray[x][y]->setPlainText(QString::number(this->engine->getAnswer(x, y)));
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
void Graphics::eraseChosenNumber()
{
    // Получаем выбранный номер и заметку из соответствующих массивов
    QString number = this->TextArray[this->chosenSquare.first.value()][this->chosenSquare.second.value()]->toPlainText();
    QString noteNumber = this->NotesArray[this->chosenSquare.first.value()][this->chosenSquare.second.value()]->toPlainText();

    // Проверяем, есть ли номер и заметка для удаления
    if (!number.isEmpty() || !noteNumber.isEmpty()) {
        // Сохраняем действия в стеке для возможности отмены
        if (!number.isEmpty()) {
            this->ActionStack.push({{chosenSquare.first.value(), chosenSquare.second.value()},
                                    Graphics::Action::TYPE_OF_ACTION::ERASE, static_cast<uint8_t>(number.toInt()), std::nullopt});
        }
        if (!noteNumber.isEmpty()) {
            this->ActionStack.push({{chosenSquare.first.value(), chosenSquare.second.value()},
                                    Graphics::Action::TYPE_OF_ACTION::DELETE_NOTE,
                                    static_cast<uint8_t>(noteNumber.toInt()), std::nullopt});
        }

        // Очищаем текстовые поля
        this->TextArray[this->chosenSquare.first.value()][this->chosenSquare.second.value()]->setPlainText("");
        this->NotesArray[this->chosenSquare.first.value()][this->chosenSquare.second.value()]->setPlainText("");

        // Возвращаемся к предыдущему состоянию (если это необходимо)
        this->comeBack();
    }
}

void Graphics::returnLastAction()
{
    // Проверяем, есть ли действия в стеке
    if (this->ActionStack.empty()) {
        return;
    }

    // Получаем последнее действие и удаляем его из стека
    Action lastAction = this->ActionStack.top();
    this->ActionStack.pop();

    uint8_t x = lastAction.coordinates.first;
    uint8_t y = lastAction.coordinates.second;

    // В зависимости от типа действия восстанавливаем состояние
    switch (lastAction.action) {
    case Action::TYPE_OF_ACTION::ERASE:
        // Восстанавливаем значение, которое было до удаления
        if (lastAction.numberWAS.has_value()) {
            this->TextArray[x][y]->setPlainText(QString::number(lastAction.numberWAS.value()));
        }
        break;

    case Action::TYPE_OF_ACTION::ADD:
        // Если добавляли число, нужно удалить его
        if (lastAction.numberWAS.has_value()) {
            this->TextArray[x][y]->setPlainText(QString::number(lastAction.numberWAS.value()));
        }else{
            this->TextArray[x][y]->setPlainText("");
        }
        break;

    case Action::TYPE_OF_ACTION::MAKE_NOTE:
        // Если добавляли заметку, нужно удалить её
        if (lastAction.numberWAS.has_value()) {
            this->NotesArray[x][y]->setPlainText(QString::number(lastAction.numberWAS.value()));
        }else{
            this->NotesArray[x][y]->setPlainText("");
        }
        break;

    case Action::TYPE_OF_ACTION::DELETE_NOTE:
        // Восстанавливаем заметку, которая была удалена
        if (lastAction.numberWAS.has_value()) {
            this->NotesArray[x][y]->setPlainText(QString::number(lastAction.numberWAS.value()));
        }
        break;

    default:
        break;

    }
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
    pen.setColor(Qt::black);
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
        }else{
            for(const auto& pair : RecentlyAdded){
                if(pair.first == x and pair.second == y)
                    goto MARK;
            }
        }
        return;
    }

MARK:
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
