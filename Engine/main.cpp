#include <QCoreApplication>
#include "game.h"
#include "gtphandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList pn;
    pn.append( "Player1" );
    pn.append( "Player2" );
          Game *gp = new Game(pn);
    GtpHandler *hp = new GtpHandler(gp);

    return a.exec();
}
