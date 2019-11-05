#include <QCoreApplication>
#include "game.h"
#include "gtphandler.h"

#include "ruikei.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList pn;
    pn.append( "Player1" );
    pn.append( "Player2" );

    new Ruikei();

    Game *gp = new Game(pn);
    new GtpHandler(&a,gp);
    return a.exec();
}

// Common variable names / usage:
// i - index onto a Goban
// j - loop control index
// k - loop control index
// x - coordinate on a Goban
// y - coordinate on a Goban
// c - color (0 = black, 1 = white, -1 = neither, -2 = not yet determined, 2+ for weird games)
// pl - player number, used when inner loops are concerned with color
// p - pointer to parent object
// bp - Goban pointer  - the board
// cp - Chiiki pointer
// dp - Draco pointer
// gp - Game pointer
// hp - Chiho pointer  - holds a region definition
// ip - Goishi pointer
// jp - Jiyu pointer
// mp - Sakudo pointer - makes moves
// pp - Player pointer
// rp - Ryoiki pointer - regions of free space
// sp - Gosu pointer
// tp - Shiko pointer  - thinks about the Goban
// wp - Wyrm pointer   - regions of connected Goishi
