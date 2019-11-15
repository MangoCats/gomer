#include <QCoreApplication>
#include <QSettings>
#include "shiai.h"
#include "gtphandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set names for QSettings use
    QCoreApplication::setOrganizationName("MangoCats");
    QCoreApplication::setOrganizationDomain("mangocats.com");
    QCoreApplication::setApplicationName("GomerEngine");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QStringList pn;
    pn.append( "Player1" );
    pn.append( "Player2" );
    Shiai *gp = new Shiai(pn);
    new GtpHandler(&a,gp);
    return a.exec();
}

// Common variable names / usage:
// i - index onto a Menseki (Goban or Ruikei)
// j - loop control index
// k - loop control index
// x - coordinate on a Menseki (Goban or Ruikei)
// y - coordinate on a Menseki (Goban or Ruikei)
// c - color (0 = black, 1 = white, -1 = neither, -2 = not yet determined, 2+ for weird games)
// pl - player number, used when inner loops are concerned with color
// p - pointer to parent object
// ap - Ruikei pointer - analysis of a piece of the Goban state
// bp - Goban pointer  - the board
// cp - Chiiki pointer
// dp - Draco pointer
// ep - Soshi pointer - data element of a Kogai
// gp - Shiai pointer - holds a game state
// hp - Chiho pointer  - holds a region definition
// ip - Goishi pointer
// jp - Jiyu pointer
// kp - Kigo pointer
// mp - Sakudo pointer - makes moves
// np - Bunkai pointer - analyzes Ruikei
// op - Kogai pointer - outline / summary of position
// pp - Player pointer
// rp - Ryoiki pointer - regions of free space
// sp - Gosu pointer
// tp - Shiko pointer  - thinks about the Goban
// vp - Dotei pointer  - vertex labels
// wp - Wyrm pointer   - regions of connected Goishi
