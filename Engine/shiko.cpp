#include "shiko.h"

/**
 * @brief Shiko::Shiko - normal constructor, called at game start with an empty Goban
 * @param pbp - passed Goban pointer, Goban this Shiko thinks about
 * @param p - parent, Game this Shiko is playing
 */
Shiko::Shiko(Shiai *p) : QObject(p), bp(p->bp), gp(p)
{ np = new Bunkai( this );
  cp = new Chiiki( this );
  jp = new Jiyu( bp );
}

/**
 * @brief Shiko::Shiko - copy constructor
 * @param tp - Shiko to copy
 * @param p - new Game parent
 */
Shiko::Shiko(Shiko *tp, Shiai *p) : QObject(p), bp(p->bp), gp(p)
{ foreach ( Wyrm *wp, tp->wpl )
    wpl.append( new Wyrm( wp, this ) );
  stateHistory = tp->stateHistory;
  np = tp->np; // all Shiko share one Bunkai
  cp = new Chiiki( tp->cp, this );
  jp = new Jiyu( tp->jp, bp );
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
 * @brief Shiko::resizeGoban - board has changed size, adjust analysis structures to match
 */
void Shiko::resizeGoban()
{ if ( cp != nullptr )
    cp->resizeGoban();
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
 * @param anyColor - if true, allow moves by not current player
 * @return true if the move would be legal
 */
bool Shiko::legalMove( qint32 x, qint32 y, qint32 c, bool anyColor )
{ if ( gp == nullptr )
    { qDebug( "WARNING: Shiko::legalMove Game null" );
      return false;
    }
  if ( bp == nullptr )
    { qDebug( "WARNING: Shiko::legalMove Goban null" );
      return false;
    }
  if (( !anyColor ) && ( c != gp->pt ))
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
 * @param anyColor - if true, allow moves by not current player
 * @return true if move is legal
 */
bool Shiko::legalMoveIndex( qint32 i, qint32 c, bool anyColor )
{ qint32 x,y;
  if ( bp == nullptr )
    { qDebug( "WARNING: Shiko::legalMoveIndex Goban null" );
      return false;
    }
  bp->indexToXY( i, &x, &y );
  return legalMove( x, y, c, anyColor );
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
  if ( x > 0 )             if ( selfCaptureRelief( x-1,y,c ) ) return false;
  if ( x < bp->Xsize()-1 ) if ( selfCaptureRelief( x+1,y,c ) ) return false;
  if ( y > 0 )             if ( selfCaptureRelief( x,y-1,c ) ) return false;
  if ( y < bp->Ysize()-1 ) if ( selfCaptureRelief( x,y+1,c ) ) return false;
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
  Wyrm *wp = ip->wp;
  if ( wp == nullptr )
    { qDebug( "UNEXPECTED: Shiko::selfCaptureRelief %d Goishi @ %d,%d Wyrm null", c, x, y );
      return false;
    }
  if (( ip->color == c ) && ( !wp->inAtari() ))
    return true; // Friendly Wyrm not in Atari
  if (( ip->color != c ) && ( wp->inAtari() ))
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
  if ( x > 0 )             koEvalCapture( x-1,y,c,&state );
  if ( x < bp->Xsize()-1 ) koEvalCapture( x+1,y,c,&state );
  if ( y > 0 )             koEvalCapture( x,y-1,c,&state );
  if ( y < bp->Ysize()-1 ) koEvalCapture( x,y+1,c,&state );
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
        state->replace( bp->xyToIndex(ip->x,ip->y),1,bp->vp->goishiChar.at(0) );
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
  if ( ip->x > 0 )                 collectWyrm( &fwpl, &owpl, ip->x-1, ip->y, ip->color );
  if ( ip->x < (bp->Xsize() - 1) ) collectWyrm( &fwpl, &owpl, ip->x+1, ip->y, ip->color );
  if ( ip->y > 0 )                 collectWyrm( &fwpl, &owpl, ip->x, ip->y-1, ip->color );
  if ( ip->y < (bp->Ysize() - 1) ) collectWyrm( &fwpl, &owpl, ip->x, ip->y+1, ip->color );

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

  cp->update(); // Update Chiiki

  evaluateLife();

  jp->update(); // Update Jiyu

  evaluateDraco();
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
  if ( x > 0 )             addCaptureLiberty( x-1,y,i,c );
  if ( x < bp->Xsize()-1 ) addCaptureLiberty( x+1,y,i,c );
  if ( y > 0 )             addCaptureLiberty( x,y-1,i,c );
  if ( y < bp->Ysize()-1 ) addCaptureLiberty( x,y+1,i,c );
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
 * @brief Shiko::testLibertyCount
 * @param i - index where Goishi might be theoretically placed
 * @param c - color of the theoretical Goishi placed there
 * @return number of liberties the resulting Wyrm would enjoy if this Goishi were so placed...
 */
qint32 Shiko::testLibertyCount( qint32 i, qint32 c )
{ qint32 x,y;
  bp->indexToXY( i, &x, &y );
  QList<qint32> lip; // List of all liberties of the new proposed Wyrm
  if ( x > 0 )               armLibertyCollect( x-1, y, c, lip );
  if ( x < bp->Xsize() - 1 ) armLibertyCollect( x+1, y, c, lip );
  if ( y > 0 )               armLibertyCollect( x, y-1, c, lip );
  if ( y < bp->Ysize() - 1 ) armLibertyCollect( x, y+1, c, lip );
  lip.removeAll(i);  // Proposed Goishi going to i, won't get that as a liberty anymore
  // QString msg = QString( "# testLibertyCount(%1,%2) " ).arg(bp->indexToVertex(i)).arg(c);
  // foreach ( qint32 j, lip )
  //   msg.append( bp->indexToVertex(j)+" " );
  // qDebug( msg.toUtf8().data() );
  return lip.size();
}

/**
 * @brief Shiko::armLibertyCollect - gather liberties from one "arm"
 * @param x - coordinate to add liberties from
 * @param y - coordinate to add liberties from
 * @param c - color to add liberties for
 * @param lip - list of liberty index points collected
 */
void Shiko::armLibertyCollect( qint32 x, qint32 y, qint32 c, QList<qint32> &lip )
{ Goishi *ip = bp->goishiAt(x,y);
  qint32  i  = bp->xyToIndex(x,y);
  if ( ip == nullptr )         // Empty grid point?
    { if ( !lip.contains(i) )  // If we haven't already got this one
        lip.append(i);         // Score a direct liberty
      return;
    }
  if ( ip->color != c )                     // Opposing Goishi?
    { if ( ip->wp->libertyList.size() < 2 ) // In Atari?
        lip.append(i);                      // Capture would make this grid point free (maybe others for friendly Wyrms too, presently neglected)
      return;                               // if not, no new liberties here.
    }
  Wyrm *wp = ip->wp;    // Friendly Wyrm.
  if ( wp == nullptr )
    { qDebug("UNEXPECTED: Shiko::armLibertyCount() Wyrm null" );
      return;
    }
  // Collect all new liberties from the neighbor Wyrm
  foreach ( qint32 wli, wp->libertyList )
    if ( !lip.contains(wli) )
      lip.append( wli );
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

QString Shiko::showDraco()
{ QString s = "\nDraco:\n";
  foreach( Draco *dp, dpl )
    s.append( dp->show() );
  return s;
}

/**
 * @brief Shiko::bensonsChiho - First, calculate Chiho around the Wyrms
 *   on Benson's board.  Then, remove any occupied by opponent on the Game board
 *   grid points from them.
 * @param bbp - Bensons working Goban, with opponent Goishi removed
 * #param c - color we are
 * @return List of Bensons pass-alive algorithm Ryoiki for the given Wyrms
 */
QList<Chiho *> Shiko::bensonsChiho( Goban *bbp, qint32 c )
{ QList<Chiho *> bhpl;
  bhpl = bbp->fillByRule( c, false );
  // Now, the Benson "special treatment" - remove points filled by opponent Goishi
  qint32 j = 0;
  while ( j < bhpl.size() )
    { Chiho *hp = bhpl.at(j);
      if ( hp == nullptr )
        qDebug( "UNEXPECTED: Shiko::bensonsChiho Chiho null" );
       else
        { qint32 k = 0;
          while ( k < hp->il.size() )
            if ( bp->color( hp->il.at(k) ) != NO_PLAYER )
              hp->il.removeAt( k );
             else
              k++;
          if ( hp->il.size() < 1 )
            { qDebug( "UNEXPECTED: Shiko::bensonsChiho Chiho empty" );
              hp->deleteLater();
              hp = nullptr;
            }
        }
      if ( hp == nullptr )
        bhpl.removeAt( j );
       else
        j++;
    }
  // qDebug( "Shiko::bensonsChiho bhpl.size() %d", bhpl.size() );
  return bhpl;
}

/**
 * @brief Shiko::onlyWyrmsColored - remove all Wyrms not colored c
 * @param bbp - Goban to operate on (NOT the Game Goban!)
 * @param c - color to match
 */
void Shiko::onlyWyrmsColored( Goban *bbp, qint32 c )
{ for ( qint32 i = 0; i < bbp->nPoints(); i++ )
    { Goishi *ip = bbp->goishi(i);
      if ( ip != nullptr )
        if ( ip->color != c )
          bbp->takeGoishi(i)->deleteLater();
    }
}

/**
 * @brief Shiko::allWyrms
 * @param bbp - Goban to read
 * @return list of all Wyrms on the Goban
 */
QList<Wyrm *> Shiko::allWyrms( Goban *bbp )
{ QList<Wyrm *>awpl;
  for ( qint32 i = 0; i < bbp->nPoints(); i++ )
     { Goishi *ip = bbp->goishi(i);
       if ( ip != nullptr )
         { Wyrm *wp = ip->wp;
           if ( wp != nullptr )
             if ( !awpl.contains( wp ) )
               awpl.append( wp );
         }
     }
  return awpl;
}

/**
 * @brief Shiko::clearWyrm
 * @param bbp - Goban to operate on - not the Game Goban
 * @param wp - Wyrm to remove from the Goban
 */
void  Shiko::clearWyrm( Goban *bbp, Wyrm *wp )
{ foreach ( Goishi *ip, wp->ipl )
    { if ( ip->bp != bbp )
        qDebug( "UNEXPECTED: Shiko::clearWyrm mismatch between Wyrm's Goishi's Goban and passed Goban" );
       else
        bbp->removeGoishi( ip );
    }
}

/**
 * @brief Shiko::isVital - is this Chiho vital to the Wyrm?
 *   Basically, all gridpoints of the Chiho must
 * @param wp - Wyrm to examine
 * @param rp - Chiho to examine
 * @return true if the Chiho is vital to the Wyrm
 */
bool Shiko::isVital( Wyrm *wp, Chiho *hp )
{ if ( wp == nullptr ) { qDebug( "Shiko::isVital Wyrm null"   ); return false; }
  if ( hp == nullptr ) { qDebug( "Shiko::isVital Chiho null" ); return false; }
  if ( hp->il.size() <= 0 ) { qDebug( "Shiko::isVital Chiho size 0" ); return false; }
  if ( wp->libertyList.size() <= 0 ) { qDebug( "Shiko::isVital Wyrm libertyList size 0" ); return false; }
  foreach ( qint32 i, hp->il )
    if ( !wp->libertyList.contains(i) )
      return false;
  return true;
}

/**
 * @brief Shiko::vitalCount - determining if a Wyrm is pass-alive by counting vital nearby Chiho
 * @param wp - Wyrm pointer
 * @param crpl - color-matched Chiho pointer list
 * @return the number of vital Chiho adjacent to wp
 */
qint32 Shiko::vitalCount( Wyrm *wp, const QList<Chiho *>& chpl )
{ qint32 vc = 0;
  foreach ( Chiho *hp, chpl )
    if ( isVital( wp, hp ) )
      vc++;
  // QString ws = wp->show(); ws.chop(1);
  // QString msg = QString( "Shiko::vitalCount(%1) %2" ).arg(ws).arg(vc);
  // qDebug( msg.toUtf8().data() );
  return vc;
}

/**
 * @brief Shiko::passEyes
 * @param wp - Wyrm to evaluate
 * @param chpl -  a fully reduced Bensons Chiho list (all false eyes removed)
 * @return list of all Goban index grid points contained in vital
 */
QList<qint32> Shiko::passEyes( Wyrm *wp, const QList<Chiho *>& chpl )
{ QList<qint32> pel;
  foreach ( Chiho *hp, chpl )
    if ( isVital( wp, hp ) )
      pel.append( hp->il );
  return pel;
}

/**
 * @brief Shiko::evaluateLife - Chiiki has been recently updated, now
 *   re-evaluate the life status of all Wyrms, first by Bensons algorithm:
 *   https://senseis.xmp.net/?BensonsAlgorithm
 *   https://webdocs.cs.ualberta.ca/~games/go/seminar/2002/020717/benson.pdf
 *     see particularly section 4 pp. 21-22
 *   Note: Benson's unconditional life rules break down for N players > 2
 *     because the additional players can effectively "escape" suicide by
 *     combination and capture of each other.
 */
void Shiko::evaluateLife()
{ // qDebug( "Shiko::evaluateLife()" );
  if ( gp == nullptr ) { qDebug( "Shiko::evaluateLife() Game null" ); return; }
  if ( bp == nullptr ) { qDebug( "Shiko::evaluateLife() Goban null" ); return; }
         qint32  np  = gp->np;
          Chiho *hp  = nullptr;
           Wyrm *wp  = nullptr;
           Wyrm *cwp = nullptr;
          Goban *bbp = nullptr;
           Shiai *tgp = nullptr;
   QList<Wyrm *> cwpl;
  QList<Chiho *> chpl;
  foreach ( wp, wpl )          // Mark all Wyrms in this Shiko as undetermined
    { wp->lifeOrDeath = WYRM_UNSETTLED;
      wp->passEyes.clear();          // Re-count eyes every time
    }
  for ( qint32 c = 0; c < np; c++ )
    { tgp = new Shiai( gp, this );
      bbp = tgp->bp;                 // Benson's Goban
      onlyWyrmsColored( bbp, c );    // Remove Wyrms not matching the current player's color from the bbp
      bool allAlive = false;         // Now, strip out the non-pass-alive Wyrms until only pass-alive Wyrms remain
      while ( !allAlive )
        { foreach ( hp, chpl )
            hp->deleteLater();       // Clean up the Chiho list from the previous pass (if any)
          chpl.clear();
          chpl = bensonsChiho( bbp, c ); // qDebug( "c %d chpl.size() == %d", c, chpl.size() );
          cwpl = allWyrms( bbp );
          // qDebug( "Shiko::evaluateLife() c %d n Wyrms %d", c, cwpl.size() );
          allAlive = true;           // assume true until proven false on this pass
          qint32 j = 0;
          while ( j < cwpl.size() )  // Examine every Wyrm colored c
            { cwp = cwpl.at(j);
              if ( vitalCount( cwp, chpl ) > 1 )
                j++;
               else
                { allAlive = false;
                  clearWyrm( bbp, cwp );
                  cwpl.removeAt(j);  // Remove any Wyrm which does not have at least two vital Ryoiki
                  cwp->deleteLater();
                }
            }
        }
      foreach ( cwp, cwpl )          // Wyrms remaining in cwpl are alive
        { wp = bp->grid.at( cwp->il.at(0) )->wp; // Go to the Shiko's Goban to get the Wyrm
          wp->passEyes    = passEyes( cwp, chpl );
          wp->lifeOrDeath = WYRM_LIVE;
          // QString msg = QString( "Shiko::vitalCount(%1) set live" ).arg(wp->show());
          // qDebug( msg.toUtf8().data() );
          cwp->deleteLater();
        }
      cwpl.clear();
      tgp->deleteLater();
    }
  foreach ( hp, chpl )
    hp->deleteLater();               // Clean up the Chiho list from the last pass
}

/**
 * @brief Shiko::evaluateDraco - at each turn, build the Draco list up
 *   from scratch, too many complexities and fuzzy connections to try
 *   to fix the existing list, particularly in the end game.
 */
void Shiko::evaluateDraco()
{ foreach ( Draco *dp, dpl )
    dp->deleteLater();
  dpl.clear();

  QVector<QList<Wyrm *> > cwpm;  // Sort Wyrms by color
  cwpm.resize( gp->np );
  foreach ( Wyrm *wp, wpl )
    { cwpm[wp->color()].append(wp);
      wp->dp = nullptr;
    }

  // Build the Draco for each color
  for ( qint32 c = 0; c < gp->np; c++ )
    foreach ( Wyrm *wp, cwpm.at(c) )
      if ( wp->dp == nullptr )
        { new Draco( wp, cwpm.at(c), this );
          dpl.append( wp->dp );
        }
}

