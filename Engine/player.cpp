#include "player.h"

Player::Player(QString n, Gosu *p) : QObject(p), name(n), sp(p)
{ // qDebug( "Player constructor %s", qPrintable( n ) );
  bp = sp->bp;
}
