#include <src/gamefield.h>
#include <iostream>
#include <QDebug>
#include "src/engine.h"

int main(int argc, char *argv[])
{
    Engine eng(GAME_LEVEL::EASY_LEVEL);
    eng.generate();
    eng.print();

    return 0;
}
