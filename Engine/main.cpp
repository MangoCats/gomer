#include <QCoreApplication>
#include "game.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Game *gp = new Game();

    return a.exec();
}
