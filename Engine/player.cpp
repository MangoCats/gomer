#include "player.h"

Player::Player(QString n, Gosu *parent) : QObject(parent), name( n )
{ qDebug( "Player constructor %s", qPrintable( n ) );
  sp = parent;
  bp = sp->bp;
}
