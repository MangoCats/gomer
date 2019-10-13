#include "game.h"

Game::Game(QStringList playerNames, qint32 xs, qint32 ys, QObject *parent) : QObject(parent), np(playerNames.size())
{ qDebug( "Game constructor %d players", np );
  // TODO: if np < 2 or np too big, fail out.
  bp = new Goban(this,xs,ys);
  for ( qint32 i = 0; i < np; i++ )
    { Gosu *sp = new Gosu(bp);
      spl.append( sp );
      ppl.append( new Player(playerNames.at(i), sp) );
    }
  qint32 nPoints = bp->nPoints();
  qint32 cc = 0;
  while ( nPoints > 0 )
    { spl.at(cc)->addGoishiToBowl( new Goishi(cc,bp) );
      if ( ++cc >= np )
        cc = 0;
      nPoints--;
    }
}

/**
 * @brief Game::clearBoard - move all Goishi into their original Gosu bowls
 */
void Game::clearBoard()
{ qint32 nPoints = bp->nPoints();
  for ( qint32 i = 0 ; i < nPoints ; i++ )
    { Goishi *ip = bp->takeGoishi( i );
      if ( ip != nullptr )
        clearGoishi( ip );
    }
  qint32 ns = spl.size();
  if ( np != spl.size() )
    qDebug( "WARNING: np %d != spl.size() %d", np, spl.size() );

  for ( qint32 i = 0 ; i < ns ; i++ )
    while ( spl.at(i)->goishiInLid() > 0 )
      clearGoishi( spl.at(i)->takeGoishiFromLid() );
}

/**
 * @brief Game::clearGoishi - take a "loose" Goishi and place it in its Gosu bowl according to color
 * @param ip - pointer to a Goishi that is not placed elsewhere
 */
void Game::clearGoishi( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "WARNING: Game::clearGoishi( nullptr )" );
      return;
    }
  qint32 c = ip->color;
  if (( c >= spl.size() ) || ( c < 0 ))
    { qDebug( "WARNING: Game::clearGoishi() Goishi color %d out of range [0,%d)",c,spl.size() );
      return;
    }
  if ( spl.at(c) == nullptr )
    { qDebug( "WARNING: Game::clearGoishi() Gosu %d is nullptr",c );
      return;
    }
  spl.at(c)->addGoishiToBowl(ip);
}
