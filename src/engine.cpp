#include "engine.h"



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
            }
            return false;
        }
    }
    return true;
}

void Engine::print()
{
    for(uint8_t i = 0; i < 9; i++){
        for(uint8_t j = 0 ; j < 9; j++){
            std::cout << this->pimpl->answer.getNumber(i,j) << " ";
        }
        std::cout << std::endl;
    }
}
