#include "game.h"

Game::Game(qint32 nPlayers, QObject *parent) : QObject(parent)
{ qDebug( "Game constructor %d players", nPlayers );
  bp = new Goban(this);
  for ( qint32 i = 0; i < nPlayers; i++ )
    spl.append( new Gosu(bp) );
  qint32 np = bp->nPoints();
  qint32 cc = 0;
  while ( np > 0 )
    { spl.at(cc)->addGoishiToBowl( new Goishi(cc,this) );
      if ( ++cc >= nPlayers )
        cc = 0;
      np--;
    }


}
