#include "game.h"

Game::Game(qint32 nPlayers, qint32 xs, qint32 ys, QObject *parent) : QObject(parent), np(nPlayers)
{ qDebug( "Game constructor %d players", nPlayers );
  bp = new Goban(this,xs,ys);
  for ( qint32 i = 0; i < nPlayers; i++ )
    { Gosu *sp = new Gosu(bp);
      spl.append( sp );
      ppl.append( new Player(sp) );
    }
  qint32 np = bp->nPoints();
  qint32 cc = 0;
  while ( np > 0 )
    { spl.at(cc)->addGoishiToBowl( new Goishi(cc,bp) );
      if ( ++cc >= nPlayers )
        cc = 0;
      np--;
    }


}
