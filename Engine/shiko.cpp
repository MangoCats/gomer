#include "shiko.h"

/**
 * @brief Shiko::Shiko - normal constructor, called at game start with an empty Goban
 * @param pbp - passed Goban pointer, Goban this Shiko thinks about
 * @param parent - Game this Shiko is playing
 */
Shiko::Shiko(Game *parent) : QObject(parent), gp(parent), bp(parent->bp)
{}

/**
 * @brief Shiko::Shiko - copy constructor
 * @param tp - Shiko to copy
 * @param parent - new Game parent
 */
Shiko::Shiko(Shiko *tp, Game *parent) : QObject(parent), gp(parent), bp(tp->bp)
{ foreach ( Wyrm *wp, tp->wpl )
    wpl.append( new Wyrm( wp, this ) );
  stateHistory = tp->stateHistory;
}

/**
 * @brief Shiko::clearGoban - reset to an empty Goban state
 */
void Shiko::clearGoban()
{ while ( wpl.size() > 0 )
    wyrmCaptured( wpl.at(0) ); // Clears the wp-s in the Goishi as well as removing the Wyrm from wpl
  stateHistory.clear();
  stateHistory.append( bp->state() ); // Move 0 state, Goban should already be clear
}

/**
 * @brief Shiko::allLegalMoves
 * @param c - color to move
 * @return list of all legal moves for color c, by index
 */
QList<qint32> Shiko::allLegalMoves( qint32 c )
{ QList<qint32> lml;
  if ( bp == nullptr )
    { qDebug( "WARNING: Shiko::allLegalMoves Goban null" );
      return lml;
    }
  qint32 x,y;
  for ( qint32 i = 0; i < bp->nPoints() ; i++ )
    { bp->indexToXY( i, &x, &y );
      if ( legalMove(x,y,c) )
        lml.append(i);
    }
  return lml;
}

/**
 * @brief Shiko::legalMove - hypothetical move, is it legal to play on the current board state?
 * @param x - coordinate to place Goishi
 * @param y - coordinate to place Goishi
 * @param c - color of Goishi to place
 * @return true if the move would be legal
 */
bool Shiko::legalMove( qint32 x, qint32 y, qint32 c )
{ if ( gp == nullptr )
    { qDebug( "WARNING: Shiko::legalMove Game null" );
      return false;
    }
  if ( bp == nullptr )
    { qDebug( "WARNING: Shiko::legalMove Goban null" );
      return false;
    }
  if ( c != gp->pt )
    { qDebug( "Shiko::legalMove() %d does not match current player turn %d", c, gp->pt );
      return false;
    }
  if ( !bp->onBoard(x,y) )
    { qDebug( "Shiko::legalMove() %d, %d is not on Goban",x,y );
      return false;
    }
  QString v = bp->xyToVertex(x,y);
  if ( bp->goishiAt(x,y) != nullptr )
    { // qDebug( "Shiko::legalMove() Goishi already present at %s", qPrintable(v) );
      return false; // Goishi already placed there
    }
  if ( isSelfCapture(x,y,c) )
    { // qDebug( "Shiko::legalMove() %s would result in self capture", qPrintable(v) );
      return false;
    }
  if ( isKo(x,y,c) )
    { // qDebug( "Shiko::legalMove() %s would result in Ko", qPrintable(v) );
      return false;
    }
  return true;
}

/**
 * @brief Shiko::legalMoveIndex
 * @param i - index of position to try to move
 * @param c - color to try to move
 * @return true if move is legal
 */
bool Shiko::legalMoveIndex( qint32 i, qint32 c )
{ qint32 x,y;
  if ( bp == nullptr )
    { qDebug( "WARNING: Shiko::legalMoveIndex Goban null" );
      return false;
    }
  bp->indexToXY( i, &x, &y );
  return legalMove( x, y, c );
}

/**
 * @brief Shiko::isSelfCapture
 * @param x
 * @param y
 * @param c
 * @return true if placing a color c Goishi at x,y would result in self capture
 */
bool Shiko::isSelfCapture( qint32 x, qint32 y, qint32 c )
{ if ( bp == nullptr )
    { qDebug( "Shiko::isSelfCapture Goban null" );
      return true;
    }
  // Check for open neighbors
  if ( x > 0 )           if ( selfCaptureRelief( x-1,y,c ) ) return false;
  if ( x < bp->Xsize-1 ) if ( selfCaptureRelief( x+1,y,c ) ) return false;
  if ( y > 0 )           if ( selfCaptureRelief( x,y-1,c ) ) return false;
  if ( y < bp->Ysize-1 ) if ( selfCaptureRelief( x,y+1,c ) ) return false;
  return true;
}

/**
 * @brief Shiko::selfCaptureRelief
 * @param x
 * @param y
 * @param c
 * @return true if this neighboring grid provides relief from self capture
 */
bool Shiko::selfCaptureRelief( qint32 x, qint32 y, qint32 c )
{ Goishi *ip = bp->goishiAt( x,y );
  if ( ip == nullptr )
    return true; // Empty grid - new liberty
  if (( ip->color == c ) && ( !ip->wp->inAtari() ))
    return true; // Friendly Wyrm not in Atari
  if (( ip->color != c ) && ( ip->wp->inAtari() ))
    return true; // Opponent Wyrm in Atari
  return false;
}

/**
 * @brief Shiko::isKo - neglecting self capture evaluation, check for Ko
 * @param x
 * @param y
 * @param c
 * @return true if the proposed move would result in Ko
 */
bool Shiko::isKo( qint32 x, qint32 y, qint32 c )
{ QString state = bp->state();
  // Evaluate the proposed move into state
  state.replace( bp->xyToIndex(x,y),1,bp->colorToChar(c) );
  if ( x > 0 )           koEvalCapture( x-1,y,c,&state );
  if ( x < bp->Xsize-1 ) koEvalCapture( x+1,y,c,&state );
  if ( y > 0 )           koEvalCapture( x,y-1,c,&state );
  if ( y < bp->Ysize-1 ) koEvalCapture( x,y+1,c,&state );
  return stateHistory.contains(state);
}

/**
 * @brief Shiko::koEvalCapture - reflect (potential) capture at x,y in state for isKo()
 * @param x
 * @param y
 * @param c
 * @param state - state to show capture in
 */
void Shiko::koEvalCapture( qint32 x, qint32 y, qint32 c, QString *state )
{ Goishi *ip = bp->goishiAt(x,y);
  if ( ip == nullptr )
    return; // Nothing to capture
  if ( ip->color == c )
    return; // Friendly Wyrm, joined not captured
  Wyrm *wp = ip->wp;
  if ( wp == nullptr )
    { qDebug( "WARNING: Shiko::koEvalCapture %d %d null Wyrm",x,y );
      return;
    }
  if ( !wp->inAtari() )
    return; // No capture here
  foreach ( ip, wp->ipl )
    { if ( ip == nullptr )
        qDebug( "WARNING: Shiko::koEvalCapture %d %d null Goishi in Wyrm",x,y );
       else
        state->replace( bp->xyToIndex(ip->x,ip->y),1,bp->goishiChar.at(0) );
    }
}

/**
 * @brief Shiko::goishiPlacedOnGoban - update Wyrms
 * @param ip - newly placed Goishi
 */
void Shiko::goishiPlacedOnGoban( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "WARNING: goishiPlacedOnGoban( Goishi == nullptr )" );
      return;
    }
  if ( bp == nullptr )
    { qDebug( "WARNING: goishiPlacedOnGoban( Goban nullptr )" );
      return;
    }
  QList<Wyrm *> fwpl; // Make a list of "friendly neighbors' Wyrms"
  QList<Wyrm *> owpl; // Make a list of "opponent neighbors' Wyrms"
  if ( ip->x > 0 )               collectWyrm( &fwpl, &owpl, ip->x-1, ip->y, ip->color );
  if ( ip->x < (bp->Xsize - 1) ) collectWyrm( &fwpl, &owpl, ip->x+1, ip->y, ip->color );
  if ( ip->y > 0 )               collectWyrm( &fwpl, &owpl, ip->x, ip->y-1, ip->color );
  if ( ip->y < (bp->Ysize - 1) ) collectWyrm( &fwpl, &owpl, ip->x, ip->y+1, ip->color );

  foreach( Wyrm *owp, owpl )
    { if ( owp->inAtari() )
        { addCaptureLiberties( owp );
          gp->capture( owp );
          owpl.removeAt( owpl.indexOf(owp) );
        }
    }

  if ( fwpl.size() == 0 ) // No friendly neighbors, new Wyrm
    wpl.append( new Wyrm(ip,this) );
   else if ( fwpl.size() == 1 )
    { if ( fwpl.at(0) == nullptr )
        { qDebug( "WARNING: Shiko::goishiPlacedOnGoban neighbor wp is nullptr" );
          return;
        }
      fwpl.at(0)->addGoishi( ip );
    }
   else
    { // Multiple friendly Wyrms will be joined by this newly placed Goishi
      fwpl.at(0)->addGoishi( ip );
      int i = 1;
      while ( i < fwpl.size() )
        mergeWyrms( fwpl.at(0), fwpl.at(i++) );
    }

  // Take away the opponent liberties at the newly placed Goishi grid location
  foreach ( Wyrm *owp, owpl )
    owp->removeLiberty( bp->xyToIndex( ip->x, ip->y ) );

  // Save this move in the state history
  stateHistory.append( bp->state() );
}

/**
 * @brief Shiko::addCaptureLiberties - wp is being captured, give liberties to all adjacent opponents of its Goishi
 * @param wp - captured Wyrm
 */
void  Shiko::addCaptureLiberties( Wyrm *wp )
{ if ( wp == nullptr )
    { qDebug( "Shiko::addCaptureLiberties() fed a nullptr Wyrm" );
      return;
    }
  foreach( Goishi *ip, wp->ipl )
    addCaptureLiberties( ip );
}

/**
 * @brief Shiko::addCaptureLiberties - ip is being captured, give liberties to all adjacent opponents
 * @param ip - captured Goishi
 */
void  Shiko::addCaptureLiberties( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "Shiko::addCaptureLiberties() fed a nullptr Goishi" );
      return;
    }
  if ( bp == nullptr )
    { qDebug( "Shiko::addCaptureLiberties() null Goban" );
      return;
    }
  qint32 x = ip->x;
  qint32 y = ip->y;
  qint32 c = ip->color;
  qint32 i = bp->xyToIndex(x,y);
  if ( x > 0 )           addCaptureLiberty( x-1,y,i,c );
  if ( x < bp->Xsize-1 ) addCaptureLiberty( x+1,y,i,c );
  if ( y > 0 )           addCaptureLiberty( x,y-1,i,c );
  if ( y < bp->Ysize-1 ) addCaptureLiberty( x,y+1,i,c );
}

/**
 * @brief Shiko::addCaptureLiberty - a neighboring Goishi of color c is being captured, add this liberty if applicable
 * @param x - coordinates of Goishi that might gain a liberty
 * @param y - coordinates of Goishi that might gain a liberty
 * @param i - index of the liberty that might be gained
 * @param c - color of the Goishi being captured
 */
void  Shiko::addCaptureLiberty( qint32 x, qint32 y, qint32 i, qint32 c )
{ Goishi *ip = bp->goishiAt( x, y );
  if ( ip == nullptr )
    return;
  if ( ip->color != c )
    if ( ip->wp != nullptr )
      ip->wp->addLiberty( i );
}

/**
 * @brief Shiko::wyrmCaptured - called from the Game which has taken care of moving the Goishi from Goban to Gosu
 *   Shiko needs to remove this Wyrm from the list of Wyrms on the board, and incidentally clear the Goishi wp-s.
 * @param wp - captured Wyrm
 */
void Shiko::wyrmCaptured( Wyrm *wp )
{ if ( wp == nullptr )
    { qDebug( "Shiko::wyrmCaptured( nullptr )" );
      return;
    }
  if ( !wpl.contains( wp ) )
    { qDebug( "ERROR: Shiko::wyrmCaptured() %s not found in wpl", qPrintable( wp->show() ) );
      return;
    }
  foreach( Goishi *ip, wp->ipl )
    if ( ip != nullptr )
      ip->wp = nullptr;
  wpl.removeAt( wpl.indexOf( wp ) );
  wp->deleteLater();
}

/**
 * @brief Shiko::wyrmAt
 * @param x - coordinate to examine
 * @param y - coordinate to examine
 * @return pointer to the Wyrm at the passed coordinate, or nullptr if no Wyrm / Goishi present
 */
Wyrm *Shiko::wyrmAt( qint32 x, qint32 y )
{ if ( bp == nullptr )        return nullptr;
  if ( !bp->onBoard( x, y ) ) return nullptr;
  Goishi *ip = bp->goishiAt( x, y );
  if ( ip == nullptr )        return nullptr;
  if ( ip->wp == nullptr )
    qDebug( "Unexpected: all Goishi should have a valid Wyrm." );
  return ip->wp;
}

/**
 * @brief Shiko::collectWyrm - collect Wyrms, sorting them into friendly and opponent lists
 * @param fwplp - Pointer to the Friendly Wyrm Pointer List
 * @param owplp - Pointer to the Opponent Wyrm Pointer List
 * @param x - coordinate to look for a new Wyrm to collect
 * @param y - coordinate to look for a new Wyrm to collect
 * @param c - friendly color, all others are opponents
 * @return color of Wyrm collected (even if already in list), or -1 if none found
 */
qint32 Shiko::collectWyrm( QList<Wyrm *> *fwplp, QList<Wyrm *> *owplp, qint32 x, qint32 y, qint32 c )
{ Wyrm *wp = wyrmAt( x, y );
  if ( wp == nullptr )
    return -1;
  if ( wp->color() == c )
    { if ( !fwplp->contains( wp ) )
        fwplp->append( wp );
      return c;
    }
  if ( !owplp->contains( wp ) )
    owplp->append( wp );
  return wp->color();
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

/**
 * @brief Shiko::showWyrms - debug tool, shows all Wyrm data
 */
QString Shiko::showWyrms()
{ QString s = "\nWyrms:\n";
  foreach( Wyrm *wp, wpl )
    s.append( wp->show() );
  return s;
}
