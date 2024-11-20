#include <src/gamefield.h>
#include <iostream>
#include "src/engine.h"
#include <QDebug>
#include "src/application.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Application app;
    app.show();

    return a.exec();
}
