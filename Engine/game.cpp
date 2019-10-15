#include "game.h"

/**
 * @brief Game::Game - the master container
 * @param playerNames - list of player names, size implies the number of players
 * @param xs - board X size
 * @param ys - board Y size
 * @param parent - QObject this Game "lives in", or nullptr.
 */
Game::Game(QStringList playerNames, qint32 xs, qint32 ys, QObject *parent) : QObject(parent), np(playerNames.size())
{ qDebug( "Game constructor %d players", np );
  // TODO: if np < 2 or np too big, fail out.

  // One Goban, one Gosu per player
  bp = new Goban(this,xs,ys);
  for ( qint32 i = 0; i < np; i++ )
    { Gosu *sp = new Gosu(bp);
      spl.append( sp );
      ppl.append( new Player(playerNames.at(i), sp) );
    }
  fillGosu();
}

/**
 * @brief Game::fillGosu - with enough Goishi to cover the Goban, distributed evenly
 */
void Game::fillGosu()
{ qint32 nPoints = bp->nPoints();
  qint32 cc = 0;
  while ( nPoints > 0 )
    { spl.at(cc)->addGoishiToBowl( new Goishi(cc,bp) );
      if ( ++cc >= np ) // Alternate between the players
        cc = 0;
      nPoints--;
    }
}

/**
 * @brief Game::clearGoban - move all Goishi into their original Gosu bowls
 */
void Game::clearGoban()
{ // First the board
  if ( bp == nullptr )
    { qDebug( "WARNING: Goban is nullptr" ); }
   else
    { qint32 nPoints = bp->nPoints();
      for ( qint32 i = 0 ; i < nPoints ; i++ )
        { Goishi *ip = bp->takeGoishi( i );
          if ( ip != nullptr )
            clearGoishi( ip );
        }
    }

  // Then the return the captured Goishi from the lids to their bowls
  qint32 ns = spl.size();
  if ( np != spl.size() )
    qDebug( "WARNING: np %d != spl.size() %d", np, spl.size() );

  for ( qint32 i = 0 ; i < ns ; i++ )
    { if ( spl.at(i) == nullptr )
        { qDebug( "WARNING: Gosu %d is nullptr", i ); }
       else
        { while ( spl.at(i)->goishiInLid() > 0 )
            clearGoishi( spl.at(i)->takeGoishiFromLid() );
        }
    }
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

/**
 * @brief Game::resizeGoban
 * @param xs - new x dimension
 * @param ys - new y dimension
 * @return true if successful
 */
bool Game::resizeGoban( qint32 xs, qint32 ys )
{ if ( xs < 1 ) return false;
  if ( ys < 1 ) return false;
  if ( xs+ys < 5 ) return false;
  if ( bp == nullptr ) return false;
  clearGoban(); // Return Goishi to the Gosu
  foreach ( Gosu *sp, spl )
    sp->empty(); // Empty the Gosu
  bool success = bp->resize( xs, ys );
  if ( success )
    fillGosu();
  return success;
}

/**
 * @brief Game::showBoard
 * @return simple board representation, might later embellish with capture, komi and other info
 */
QString Game::showBoard()
{ if ( bp == nullptr )
    { qDebug( "WARNING: bp null in Game::showBoard()" );
      return "";
    }
  return bp->showBoard();
}
