#include <QCoreApplication>
#include "game.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList pn;
    pn.append( "Player1" );
    pn.append( "Player2" );
    Game *gp = new Game(pn);

    return a.exec();
}
