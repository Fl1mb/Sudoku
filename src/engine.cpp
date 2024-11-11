#include "engine.h"
#include <QDebug>


Engine::Engine(GAME_LEVEL lvl_of_game): pimpl(std::make_unique<Engine::IMPL>())
{
    this->pimpl->level_of_game = lvl_of_game;
    this->pimpl->isGameOver = false;
}



void Engine::generate()
{
    for(uint8_t i = 0 ; i < SIZE;i += SUBGRID_SIZE){
        std::vector<uint8_t> nums {1,2,3,4,5,6,7,8,9};
        std::shuffle(nums.begin(), nums.end(), std::mt19937{std::random_device{}()});
        for(uint8_t j = 0; j < SUBGRID_SIZE; j++){
            for(uint8_t k = 0 ;k < SUBGRID_SIZE; k++){
                this->pimpl->answer.setNumber(i + j, i + k, nums[j*SUBGRID_SIZE + k]);
            }
        }
    }
    this->fillSudoku();
}

void Engine::clear()
{
    this->pimpl.reset();
    this->pimpl = std::make_unique<Engine::IMPL>();
}

bool Engine::isSafe(uint8_t row, uint8_t col, uint8_t val)
{
    for(int x = 0 ;x < SIZE; x ++)
        if(pimpl->answer.getNumber(row, x) == val or pimpl->answer.getNumber(x, col) == val)return false;

    uint8_t rowStart = row - row % SUBGRID_SIZE;
    uint8_t colStart = col - col % SUBGRID_SIZE;

    for(uint8_t i = 0; i < SUBGRID_SIZE; i++){
        for(uint8_t j = 0; j < SUBGRID_SIZE; j++){
            if(this->pimpl->answer.getNumber(i + rowStart, j + colStart) == val)return false;
        }
    }
    return true;

}

bool Engine::fillSudoku()
{
    for(uint8_t i = 0; i < SIZE; i++){
        for(uint8_t j = 0 ; j < SIZE; j++){
            if(this->pimpl->answer.isEmpty(i,j)){
                for(uint8_t num = 1; num <= SIZE; num++){
                    if(this->isSafe(i,j,num)){
                        this->pimpl->answer.setNumber(i,j, num);
                        if(this->fillSudoku())return true;
                    }
                    this->pimpl->answer.setEmpty(i, j);
                }
                return false;
            }
        }
    }
    return true;
}

bool Engine::isValid()
{
    std::unordered_set<uint8_t> set;

    // Проверка строк и столбцов
    for (uint8_t row = 0; row < SIZE; row++) {
        set.clear(); // Очищаем множество для каждой строки
        for (uint8_t col = 0; col < SIZE; col++) {
            uint8_t num = this->pimpl->answer.getNumber(row, col);
            if (num != 0) { // Предполагаем, что 0 означает пустую ячейку
                if (set.find(num) != set.end()) {
                    return false; // Дубликат в строке
                }
                set.insert(num);
            }
        }

        set.clear(); // Очищаем множество для каждой колонки
        for (uint8_t col = 0; col < SIZE; col++) {
            uint8_t num = this->pimpl->answer.getNumber(col, row);
            if (num != 0) {
                if (set.find(num) != set.end()) {
                    return false; // Дубликат в колонке
                }
                set.insert(num);
            }
        }
    }

    // Проверка подрешеток
    for (uint8_t subgridRow = 0; subgridRow < SUBGRID_SIZE; subgridRow++) {
        for (uint8_t subgridCol = 0; subgridCol < SUBGRID_SIZE; subgridCol++) {
            set.clear(); // Очищаем множество для каждой подрешетки
            for (uint8_t i = 0; i < SUBGRID_SIZE; i++) {
                for (uint8_t j = 0; j < SUBGRID_SIZE; j++) {
                    uint8_t num = this->pimpl->answer.getNumber(subgridRow * SUBGRID_SIZE + i, subgridCol * SUBGRID_SIZE + j);
                    if (num != 0) {
                        if (set.find(num) != set.end()) {
                            return false; // Дубликат в подрешетке
                        }
                        set.insert(num);
                    }
                }
            }
        }
    }

    return true; // Если все проверки пройдены, возвращаем true
}

std::unordered_set<std::pair<int, int>, pair_hash> Engine::getRandomPairs()
{
    std::unordered_set<std::pair<int, int>, pair_hash> result;
    std::random_device rd;
    std::mt19937 gen(rd());

    int min = 0, max = 8;

    std::uniform_int_distribution<> dis(min, max);

    while(result.size() != static_cast<int>(this->pimpl->level_of_game)){
        result.insert({dis(gen), dis(gen)});
    }
    return result;
}

void Engine::print()
{
    for(uint8_t i = 0; i < 9; i++){
        for(uint8_t j = 0 ; j < 9; j++){
            qDebug() << this->pimpl->answer.getNumber(i,j) << " ";
        }
        std::cout << std::endl;
    }
}

void Engine::makeGameField()
{
    std::unordered_set<std::pair<int,int>, pair_hash> pairs{this->getRandomPairs()};
    this->pimpl->field = this->pimpl->answer;

    for(const auto& pair : pairs){
        this->pimpl->field.setEmpty(static_cast<uint8_t>(pair.first), static_cast<uint8_t>(pair.second));
    }


}

std::array<std::array<std::optional<uint8_t>, 9>, 9> Engine::getFieldArray() const
{
    return this->pimpl->field.getArray();
}

GameField& Engine::getGameField() const
{
    return this->pimpl->field;
}

uint8_t Engine::getAnswer(uint8_t row, uint8_t col) const
{
    return this->pimpl->answer.getNumber(row, col);
}


