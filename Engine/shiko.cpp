#include "shiko.h"

Shiko::Shiko(Goban *pbp, Game *parent) : QObject(parent), gp(parent), bp(pbp)
{

}

/**
 * @brief Shiko::clearGoban - reset to an empty Goban state
 */
void Shiko::clearGoban()
{ wpl.clear();

}

/**
 * @brief Shiko::legalMove - hypothetical move, is it legal to play on the current board state?
 * @param x - coordinate to place Goishi
 * @param y - coordinate to place Goishi
 * @param c - color of Goishi to place
 * @return true if the move would be legal
 */
bool Shiko::legalMove( qint32 x, qint32 y, qint32 c )
{ if ( c != gp->pt )
    { qDebug( "Shiko::legalMove %d does not match current player turn %d", c, gp->pt );
      return false;
    }
  if ( !bp->onBoard(x,y) )
    { qDebug( "Shiko::legalMove %d, %d is not on Goban",x,y );
      return false;
    }
  if ( bp->goishiAt(x,y) != nullptr )
    { qDebug( "Shiko::legalMove Goishi already present at %d, %d", x, y );
      return false; // Goishi already placed there
    }

  // TODO: Check for self capture

  // TODO: Check gp->stateHistory for Ko

  return true;
}

/**
 * @brief Shiko::goishiPlacedOnGoban - update Wyrms
 * @param ip - newly placed Goishi
 */
void Shiko::goishiPlacedOnGoban( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "WARNING: goishiPlacedOnGoban( Goishi == nullptr)" );
      return;
    }
  QList<Goishi *> nipl; // Make a list of "friendly neighbors"
  Goishi *nip;
  if ( ip->x > 0 )
    { nip = bp->goishiAt( ip->x-1, ip->y );
      if ( nip != nullptr ) if ( nip->color == ip->color ) nipl.append( nip );
    }
  if ( ip->x < (bp->Xsize - 1) )
    { nip = bp->goishiAt( ip->x+1, ip->y );
      if ( nip != nullptr ) if ( nip->color == ip->color ) nipl.append( nip );
    }
  if ( ip->y > 0 )
    { nip = bp->goishiAt( ip->x, ip->y-1 );
      if ( nip != nullptr ) if ( nip->color == ip->color ) nipl.append( nip );
    }
  if ( ip->y < (bp->Ysize - 1) )
    { nip = bp->goishiAt( ip->x, ip->y+1 );
      if ( nip != nullptr ) if ( nip->color == ip->color ) nipl.append( nip );
    }
  if ( nipl.size() == 0 )
    { wpl.append( new Wyrm(ip,this) );
      return; // No friendly neighbors, new Wyrm
    }
  if ( nipl.size() == 1 )
    { if ( nipl.at(0)->wp == nullptr )
        { qDebug( "WARNING: Shiko::goishiPlacedOnGoban neighbor wp is nullptr" );
          return;
        }
      nipl.at(0)->wp->addGoishi( ip );
      return;
    }
  // Multiple friendly Wyrms will be joined by this newly placed Goishi
  nipl.at(0)->wp->addGoishi( ip );
  int i = 1;
  while ( i < nipl.size() )
    mergeWyrms( nipl.at(0)->wp, nipl.at(i++)->wp );
}

/**
 * @brief Shiko::mergeWyrms - merge two Wyrms
 * @param wp - Wyrm to merge to
 * @param wp2 - Wyrm to merge, will be removed from list
 */
void Shiko::mergeWyrms( Wyrm *wp, Wyrm *wp2 )
{ if (( wp == nullptr ) || ( wp2 == nullptr ))
    { qDebug( "mergeWyrms will not work with nullptrs" );
      return;
    }
  if ( wp->color() != wp2->color() )
    { qDebug( "WARNING: mergeWyrms color mismatch" );
      return;
    }
  wp->addWyrm( wp2 );
  int i = wpl.indexOf( wp2 );
  if ( i < 0 )
    { qDebug( "WARNING: mergeWyrms wp2 not found in wpl" );
      return;
    }
  wpl.removeAt( i );
}
