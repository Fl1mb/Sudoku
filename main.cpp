#include <src/gamefield.h>
#include <iostream>
#include "src/engine.h"
#include <QDebug>
#include "src/sudoku.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Sudoku sudoku;
    sudoku.show();

    return a.exec();
}
