#include "game.h"

/**
 * @brief Game::Game - the master container
 * @param playerNames - list of player names, size implies the number of players
 * @param xs - board X size
 * @param ys - board Y size
 * @param parent - QObject this Game "lives in", or nullptr.
 */
Game::Game(QStringList playerNames, qint32 xs, qint32 ys, QObject *parent) : QObject(parent), np(playerNames.size()), pt(0)
{ // qDebug( "Game constructor %d players", np );
  // TODO: if np < 2 or np too big, fail out.

  // One Goban, one Gosu per player
  bp = new Goban(this,xs,ys);
  for ( qint32 i = 0; i < np; i++ )
    { Gosu *sp = new Gosu(bp);
      spl.append( sp );
      ppl.append( new Player(playerNames.at(i), sp) );
    }
  fillGosu();
  tp = new Shiko(bp,this);
}

/**
 * @brief Game::Game - copy constructor
 * @param gp - object to copy
 * @param parent - parent of the new Game object
 */
Game::Game(Game *gp, QObject *parent) : QObject(parent)
{ bp = new Goban( gp->bp, this );
  foreach ( Player *pp, gp->ppl )
    { Gosu *sp = new Gosu( pp->sp, bp );
      spl.append( sp );
      ppl.append( new Player( pp->name, sp ) );
    }
  komi         = gp->komi;
  np           = gp->np;
  pt           = gp->pt;
  tp = new Shiko( gp->tp, this );
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

  // Clear all thoughts about the Goban state
  if ( tp == nullptr )
    qDebug( "WARNING: Shiko is nullptr" );
   else
    tp->clearGoban();
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

/**
 * @brief Game::playGoishi
 * @param x - coordinate to play at
 * @param y - coordinate to play at
 * @param c - color to play
 * @return true if successful
 */
bool Game::playGoishi( qint32 x, qint32 y, qint32 c )
{ if ( tp == nullptr )
    { qDebug( "WARNING: Game::playGoishi Shiko pointer is null" );
      return false;
    }
  if ( !tp->legalMove(x,y,c) )
    return false;
  if ( c >= spl.size() )
    { qDebug( "WARNING: Game::playGoishi(%d,%d,%d) no Gosu %d available",x,y,c,c );
      return false;
    }
  if ( spl.at(c) == nullptr )
    { qDebug( "WARNING: Game::playGoishi(%d,%d,%d) Gosu %d is nullptr",x,y,c,c );
      return false;
    }
  Goishi *ip = spl.at(c)->takeGoishiFromBowl();
  if ( ip->color != c )
    { qDebug( "UNEXPECTED: wrong color %d Goishi in bowl %d", ip->color, c );
      return false;
    }
  if ( ip == nullptr )
    { qDebug( "Game::playGoishi Gosu bowl empty?  Creating new Goishi." );
      ip = new Goishi(c,bp);
    }
  if ( !bp->placeGoishiAt( ip, x, y ) )
    { qDebug( "Game::playGoishi problem during Goban::placeGoishiAt()" );
      spl.at(c)->addGoishiToBowl( ip ); // Returning Goishi to bowl it was taken from
      return false;
    }

  tp->goishiPlacedOnGoban( ip ); // Updates Wyrms

  advancePlayer();

  return true;
}
/**
 * @brief Game::advancePlayer - Advance player turn to the next player, reset to player 0 when last player has played
 */
void Game::advancePlayer()
{ if ( ++pt >= np )
    pt = 0;
}

/**
 * @brief Game::pass - handle the pass move
 */
void Game::pass()
{ advancePlayer();
  // TODO: check for all players passed, end of game
}

/**
 * @brief Game::capture - remove the Wyrm's stones from the board to the appropriate Gosu lid
 * @param wp - pointer to the captured Wyrm
 */
void Game::capture( Wyrm *wp )
{ if ( wp == nullptr )
    { qDebug( "Game::capture fed a nullptr Wyrm" );
      return;
    }
  if ( bp == nullptr )
    { qDebug( "Game::capture Goban null" );
      return;
    }
  foreach( Goishi *ip, wp->ipl )
    { if ( ip == nullptr )
        qDebug( "Game::capture Wyrm contains a nullptr Goishi" );
       else
        { bp->removeGoishi( ip );
          spl.at( pt )->addGoishiToLid( ip );
        }
    }
  tp->wyrmCaptured( wp );
}
