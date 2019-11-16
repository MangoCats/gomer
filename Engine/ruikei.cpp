#include "ruikei.h"

/**
 * @brief Ruikei::Ruikei - default constructor
 * @param xs - number of rows in this Ruikei
 * @param ys - number of columns in this Ruikei
 * @param p - parent Shiko
 */
Ruikei::Ruikei( qint32 xs, qint32 ys, Shiko *p ) : Menseki(xs,ys,&(p->gp->v)), tp(p), ap(nullptr)
{ op = new Kogai(this);
  nEdge =
  eEdge =
  wEdge =
  sEdge = 0;
  kl.resize( nPoints() );

  friendlyColor = NO_PLAYER;
  xo            =
  yo            =
  depth         =
  justCaptured  = 0;
  previousMove  = MOVE_UNDEFINED_INDEX;
}

/**
 * @brief Ruikei::Ruikei - constructs the Ruikei from a data stream
 * @param ds - data stream to read the Ruikei in from
 * @param p - parent Shiko
 */
Ruikei::Ruikei( QDataStream &ds, Shiko *p ) : Menseki(&(p->gp->v)), tp(p), ap(nullptr)
{ quint8 version;
  ds >> version;
  if ( version != 1 )
    { qDebug( "Unknown version %d while reading Ruikei", version );
    }
   else
    { ds >> rows;
      ds >> columns;
      ds >> nEdge;
      ds >> eEdge;
      ds >> wEdge;
      ds >> sEdge;
      kl.resize( nPoints() );
      bool ok = true;
      for ( qint32 i = 0; i < nPoints(); ++i )
        ok &= kl[i].fromDataStream( ds );
      if ( !ok )
        qDebug( "Ruikei::Ruikei() problem reading Kigo list" );
       else
        op = new Kogai( ds, this );
    }

  friendlyColor = NO_PLAYER; // Could go either way, at this point
  xo            =
  yo            =
  depth         =
  justCaptured  = 0;
  previousMove  = MOVE_UNDEFINED_INDEX;
}

/**
 * @brief Ruikei::Ruikei - play a move constructor,
 *   If legal, add a friendly Goishi at i, then invert
 *   the friendly/opponent indications for the next move.
 * @param pap - previous Ruikei to play on
 * @param i - gridpoint to play a friendly Goishi at
 */
Ruikei::Ruikei( Ruikei *pap, qint32 i ) : Menseki(&(pap->tp->gp->v)), tp(pap->tp), ap(pap)
{ if ( !legalFriendlyMove( i ) )
    { op = nullptr;
      return; // Ruikei is invalid (rows, columns == -1), indicating move did not happen
    }
  rows         = ap->rows;
  columns      = ap->columns;
  nEdge        = ap->nEdge;
  eEdge        = ap->eEdge;
  wEdge        = ap->wEdge;
  sEdge        = ap->sEdge;
  kl           = ap->kl;

  xo           = ap->xo;
  yo           = ap->yo;
  depth        = ap->depth + 1;
  orientation  = ap->orientation;
  op           = new Kogai( this ); // Empty Kogai, use Kogai to pass back results...
  previousMove = i;

  justCaptured = playAt( i ); // populate justCaptured as appropriate
  swapFriendlyOpponent();
}

/**
 * @brief Ruikei::playAt
 * @param i - index to play a friendly Goishi at
 * @return number of opponent Goishi captured in this move
 */
qint32 Ruikei::playAt( qint32 i )
{ if ( i == MOVE_PASS_INDEX )
    return 0;
  if (( i < 0 ) || ( i >= kl.size() ))
    { qDebug( "UNEXPECTED: Ruikei::playAt( %d ) range[0,%d)", i, kl.size() );
      return 0;
    }
  if ( !kl[i].emptyGrid )      qDebug( "UNEXPECTED: Ruikei::playAt( %d ) emptyGrid was false", i );
  if (  kl[i].opponentGoishi ) qDebug( "UNEXPECTED: Ruikei::playAt( %d ) opponentGoishi was true", i );
  if (  kl[i].friendlyGoishi ) qDebug( "UNEXPECTED: Ruikei::playAt( %d ) friendlyGoishi was true", i );
  kl[i].friendlyGoishi = true;
  kl[i].opponentGoishi = false;
  kl[i].emptyGrid      = false;
  Chiho h;
  qint32 nc = 0;
  qint32 x,y;
  indexToXY(i,&x,&y);
  if ( x > 0           ) { nc += attemptCapture( x-1,y,h ); h.clear(); }
  if ( x < (Xsize()-1) ) { nc += attemptCapture( x+1,y,h ); h.clear(); }
  if ( y > 0           ) { nc += attemptCapture( x,y-1,h ); h.clear(); }
  if ( y < (Ysize()-1) ) { nc += attemptCapture( x,y+1,h ); h.clear(); }
  return nc;
}

/**
 * @brief Ruikei::attemptCapture - recursive
 * @param x - coordinate to check for 0 liberties and capture if true
 * @param y - coordinate to check for 0 liberties and capture if true
 * @param h - Chiho tracking the opponent Wyrm Goishi visited thus far
 * @return number of Goishi successfully captured - 0 if any liberty is found
 * TODO: beef up the Kigo with information about number of off Ruikei Goishi with 0 liberties,
 *   so this capture info can be more accurate
 */
qint32 Ruikei::attemptCapture( qint32 x, qint32 y, Chiho &h )
{ qint32 i = xyToIndex( x,y );
  if ( h.contains( i ) )
    return h.il.size();
  if ( ( kl.at(i).outsideOpponentLiberty ) ||
       ( kl.at(i).emptyGrid ) )
    return 0;
  if ( kl.at(i).opponentGoishi )
    { h.addIndex( i );
      if ( x > 0           ) if ( attemptCapture( x-1,y,h ) < 1 ) return 0;
      if ( x < (Xsize()-1) ) if ( attemptCapture( x+1,y,h ) < 1 ) return 0;
      if ( y > 0           ) if ( attemptCapture( x,y-1,h ) < 1 ) return 0;
      if ( y < (Ysize()-1) ) if ( attemptCapture( x,y+1,h ) < 1 ) return 0;
      kl[i].opponentGoishi = false;
      kl[i].emptyGrid      = true;
      return h.il.size();
    }
  return h.il.size();
}

/**
 * @brief Ruikei::swapFriendlyOpponent - swap friendly for opponent in all elements
 */
void Ruikei::swapFriendlyOpponent()
{ for ( qint32 i = 0; i < kl.size(); i++ )
    kl[i].swapFriendlyOpponent();
  if ( ++friendlyColor >= tp->gp->np )
    friendlyColor = 0;
}


/**
 * @brief Ruikei::legalFriendlyMove
 * @param i - index where friendly Goishi is to be placed
 * @return true if this would be a legal move
 */
bool  Ruikei::legalFriendlyMove( qint32 i )
{ if ( i == MOVE_PASS_INDEX )
    return true;  // Nothing to evaluate about a pass
  if (( i < 0 ) || ( i >= kl.size() ))
    { qDebug( "Ruikei::legalFriendlyMove( (%d,%d), %d ) out of bounds", Xsize(),Ysize(),i );
      return false;
    }
  if (( !kl.at(i).emptyGrid      ) ||
      (  kl.at(i).friendlyGoishi ) ||
      (  kl.at(i).opponentGoishi ) ||
      (  kl.at(i).ko             ) )  // grid already occupied, or ko?
    return false;
  qint32 x,y;
  indexToXY( i, &x, &y );
  if (( x == 0           ) && !x0Edge()   ) return false; // Not legal to play on edge of Ruikei, unless it is also edge of Goban
  if (( x == (Xsize()-1) ) && !xSizeEdge()) return false; // Not legal to play on edge of Ruikei, unless it is also edge of Goban
  if (( y == 0           ) && !y0Edge()   ) return false; // Not legal to play on edge of Ruikei, unless it is also edge of Goban
  if (( y == (Ysize()-1) ) && !ySizeEdge()) return false; // Not legal to play on edge of Ruikei, unless it is also edge of Goban
  // Check for self-capture
  // Try move then check for ko up the ap chain...
  return true;
}

/**
 * @brief Ruikei::isValid
 * @return true if the data elements in this Ruikei are all valid
 */
bool  Ruikei::isValid() const
{ if ( rows < 3 ) return false;
  if ( columns < 3 ) return false;
  if ( orientation < 0 ) return false;
  if ( orientation > 7 ) return false;
  if ( kl.size() < nPoints() ) return false;
  for ( qint32 i = 0; i < nPoints(); ++i )
    if ( !kl.at(i).isValid() ) return false;
  if ( op == nullptr ) return false;
  return op->isValid();
}

/**
 * @brief Ruikei::toDataStream
 * @param ds - DataStream to serialize this Ruikei to
 */
void Ruikei::toDataStream( QDataStream &ds ) const
{ quint8 version = 1;
  ds << version;
  ds << rows;
  ds << columns;
  ds << nEdge;
  ds << eEdge;
  ds << wEdge;
  ds << sEdge;
  for ( qint32 i = 0; i < nPoints(); ++i )
    kl.at(i).toDataStream( ds );
  if ( op != nullptr )
    op->toDataStream( ds );
}

/**
 * @brief Ruikei::nPreviousPass
 * @return the number of pass moves that have been played previous to this Ruikei
 */
qint32  Ruikei::nPreviousPass()
{ qint32 npp = (previousMove == MOVE_PASS_INDEX) ? 1 : 0;
  if ( ap != nullptr )
    npp += ap->nPreviousPass();
  return npp;
}


/**
 * @brief Ruikei::nEdges
 * @return the number of Goban edges this Ruikei touches 0-4
 */
qint32 Ruikei::nEdges() const
{ qint32 n = 0;
  if ( nEdge ) ++n;
  if ( eEdge ) ++n;
  if ( wEdge ) ++n;
  if ( sEdge ) ++n;
  return n;
}

qint32 Ruikei::nXedges() const
{ qint32 n = 0;
  if ( x0Edge()    ) ++n;
  if ( xSizeEdge() ) ++n;
  return n;
}

qint32 Ruikei::nYedges() const
{ qint32 n = 0;
  if ( y0Edge()    ) ++n;
  if ( ySizeEdge() ) ++n;
  return n;
}

bool  Ruikei::cornerEdges() const
{ if ( x0Edge()    && y0Edge()    ) return true;
  if ( x0Edge()    && ySizeEdge() ) return true;
  if ( xSizeEdge() && y0Edge()    ) return true;
  if ( xSizeEdge() && ySizeEdge() ) return true;
  return false;
}

/**
 * @brief Ruikei::x0Edge
 * @return true if there is a Goban edge below x0 in the present orientation
 */
bool  Ruikei::x0Edge() const
{ switch ( orientation )
    { case 0:
      case 6: return wEdge;
      case 1:
      case 5: return nEdge;
      case 2:
      case 4: return eEdge;
      case 3:
      case 7: return sEdge;
    }
  return false;
}


/**
 * @brief Ruikei::y0Edge
 * @return true if there is a Goban edge below y0 in the present orientation
 */
bool  Ruikei::y0Edge() const
{ switch ( orientation )
    { case 0:
      case 4: return sEdge;
      case 1:
      case 7: return wEdge;
      case 2:
      case 6: return nEdge;
      case 3:
      case 5: return eEdge;
    }
  return false;
}

/**
 * @brief Ruikei::xSizeEdge
 * @return true if there is a Goban edge at xSize in the present orientation
 */
bool  Ruikei::xSizeEdge() const
{ switch ( orientation )
    { case 0:
      case 6: return eEdge;
      case 1:
      case 5: return sEdge;
      case 2:
      case 4: return wEdge;
      case 3:
      case 7: return nEdge;
    }
  return false;
}

/**
 * @brief Ruikei::ySizeEdge
 * @return true if there is a Goban edge at ySize in the present orientation
 */
bool  Ruikei::ySizeEdge() const
{ switch ( orientation )
    { case 0:
      case 4: return nEdge;
      case 1:
      case 7: return eEdge;
      case 2:
      case 6: return sEdge;
      case 3:
      case 5: return wEdge;
    }
  return false;
}

/**
 * @brief Ruikei::matchWyrm
 * @param wp - Wyrm to try to match to a single Wyrm pattern
 * @param bp - Goban the Wyrm resides on, must also match pattern
 * @param color - color of Goishi to consider "friendly"
 * @return true if a match is
 */
bool Ruikei::matchWyrm( Wyrm *wp, Goban *bp, qint32 color )
{ friendlyColor = color;
  for ( qint32 i = 0; i < 7; i++ )
    { orientation = i;
      if ( matchChihoInCurrentOrientation( wp, bp ) )
        return true;
    }
  return false;
}

/**
 * @brief Ruikei::matchChihoInCurrentOrientation
 * @param hp - Chiho (Wyrm, or disjoint points) to try to fit onto this Ruikei
 * @param bp - Context of the Wyrm (Goban position)
 * @return true if this Ruikei is a match for the Wyrm and surrounding Goban grid points/Goishi and edges
 */
bool Ruikei::matchChihoInCurrentOrientation( Chiho *hp, Goban *bp )
{ qint32 xs,ys,minX,maxX,minY,maxY;
  bp->chihoXYlimits( hp,minX,minY,maxX,maxY );
  xs = maxX - minX + 1; if ( xs < 0 ) xs = 0; if ( xs > bp->Xsize() ) xs = bp->Xsize();
  ys = maxY - minY + 1; if ( ys < 0 ) ys = 0; if ( ys > bp->Ysize() ) ys = bp->Ysize();
  if ( xs > Xsize() + 2 - nXedges() )          // does the Wyrm fit in this Ruikei?
    return false;                              // nope.
  if ( ys > Ysize() + 2 - nYedges() )          // does the Wyrm fit in this Ruikei?
    return false;                              // nope.
  if ( Xsize() + 2 - nXedges() > bp->Xsize() ) // does the Ruikei fit on the Goban?
    return false;                              // nope.
  if ( Ysize() + 2 - nYedges() > bp->Ysize() ) // does the Ruikei fit on the Goban?
    return false;                              // nope.

  qint32 ne = nEdges();               // Number of Edges represented in this Ruikei
  qint32 xsMin,xsMax,ysMin,ysMax;     // Search limits

  bool te = bp->chihoOnEdge( hp );    // Wyrm touching any edge?
  if ( ne == 0 )                      // Ruikei not constrained to any edge, search the middle
    { if ( te ) return false;         // Impossible to match if the Wyrm is touching an edge
      // TODO: verify +2s below, need to draw it out
      xsMin = maxX + 2 - Xsize(); if ( xsMin           <  0           ) xsMin = 0;
      xsMax = minX - 1;           if ( xsMax + Xsize() >= bp->Xsize() ) xsMax = bp->Xsize() - Xsize();
      ysMin = maxY + 2 - Xsize(); if ( ysMin           <  0           ) ysMin = 0;
      ysMax = minY - 1;           if ( ysMax + Ysize() >= bp->Ysize() ) ysMax = bp->Ysize() - Ysize();
      for ( yo = ysMin; yo <= ysMax; yo++ )
        for ( xo = xsMin; xo <= xsMax; xo++ )
          if ( matchInPosition( bp ) )
            return true;
      return false;
    }

  if ( ne == 1 )
    { xsMin = maxX + 2 - Xsize(); if ( xsMin           <  0           ) xsMin = 0;
      xsMax = minX - 1;           if ( xsMax + Xsize() >= bp->Xsize() ) xsMax = bp->Xsize() - Xsize();
      ysMin = maxY + 2 - Xsize(); if ( ysMin           <  0           ) ysMin = 0;
      ysMax = minY - 1;           if ( ysMax + Ysize() >= bp->Ysize() ) ysMax = bp->Ysize() - Ysize();
      if ( x0Edge()    ) xsMin = xsMax = 0;
      if ( xSizeEdge() ) xsMin = xsMax = bp->Xsize() - Xsize();
      if ( y0Edge()    ) ysMin = ysMax = 0;
      if ( ySizeEdge() ) ysMin = ysMax = bp->Ysize() - Ysize();
      for ( yo = ysMin; yo <= ysMax; yo++ )
        for ( xo = xsMin; xo <= xsMax; xo++ )
          if ( matchInPosition( bp ) )
            return true;
    }

  if ( ne == 2 )
    { if ( !cornerEdges() )
        { // TODO: weird side to side constrained case
          // linear search for match like a single edge, but a tiny bit different
          return false;
        }
      // Corner case gets one chance to match - 4 kinds of corners covered below
      xo = x0Edge() ? 0 : bp->Xsize() - Xsize();
      yo = y0Edge() ? 0 : bp->Ysize() - Ysize();
      return matchInPosition( bp );
    }

  if ( ne == 3 ) // Fully constrained, one chance to match
    { if ( x0Edge() && xSizeEdge() && ( bp->Xsize() != Xsize() ) ) return false;
      if ( y0Edge() && ySizeEdge() && ( bp->Ysize() != Ysize() ) ) return false;
      xo = x0Edge() ? 0 : bp->Xsize() - Xsize();
      yo = y0Edge() ? 0 : bp->Ysize() - Ysize();
      return matchInPosition( bp );
    }

  if ( ne == 4 )                                                                    // Rare, I'm guessing.
    { if (( bp->Xsize() != Xsize() ) || ( bp->Ysize() != Ysize() )) return false;   // But, easy to check
      xo = yo = 0;                                                                  // Single chance to match
      return matchInPosition( bp );
    }

  qDebug( "UNEXPECTED: Ruikei::matchWyrmInCurrentOrientation() ne == %d", ne );
  return false;
}

/**
 * @brief Ruikei::matchInPosition
 * @param bp - Goban to compare to this Ruikei
 * @return true if the Wyrm on Goban is a match for this Ruikei at offset xo,yo into the Goban (at "orientation")
 */
bool Ruikei::matchInPosition( Goban *bp )
{ for ( qint32 y = 0; y < Ysize(); y++ )
    for ( qint32 x = 0; x < Xsize(); x++ )
      if ( !matchOneGoishi( bp->goishiAt( x+xo, y+yo ), kigoAt( x,y ) ) )
        return false;
  return true;
}

/**
 * @brief Ruikei::kigoAt
 * @param x - coordinate to get Kigo from
 * @param y - coordinate to get Kigo from
 * @return copy of the Kigo @ x,y
 */
Kigo Ruikei::kigoAt( qint32 x, qint32 y )
{ qint32 i = xyToIndex( x, y );
  if (( i < 0 ) || ( i >= kl.size() ))
    { qDebug( "unexpected Ruikei::kigoAt( %d, %d ) out of range",x,y );
      return Kigo(this);
    }
  return kl.at(i);
}

/**
 * @brief Ruikei::matchOneGoishi - considering a single grid point
 * @param ip - Goishi on the Goban at this point (or nullptr if Goban grid is empty)
 * @param k - the Kigo to compare
 * @return true if the Goishi constitutes a match for the Ruikei at x,y
 * TODO: pass in Ko with the Goban somehow and consider in the emptyGrid case...
 */
bool Ruikei::matchOneGoishi( Goishi *ip, const Kigo &k )
{ if ( ip == nullptr )
    return k.emptyGrid;
  if (( ip->color == friendlyColor ) && ( k.friendlyGoishi ))
    return true;
  if (( ip->color != friendlyColor ) && ( k.opponentGoishi ))
    return true;
  return false;
}

/**
 * @brief matchBoth
 * @param wp1 - Wyrm to try to match to a two Wyrm pattern
 * @param wp2 - Wyrm to try to match to a two Wyrm pattern
 * @param bp - Goban the Wyrm resides on, must also match pattern
 * @return true if a match is
 */
bool Ruikei::matchBoth( Wyrm *wp1, Wyrm *wp2, Goban *bp )
{ return false;
}

/**
 * @brief Ruikei::nGoishi
 * @param friendly - if true, count friendly Goishi, if false count opponent's
 * @return number of Goishi in the entire Ruikei
 */
qint32  Ruikei::nGoishi( bool friendly )
{ qint32 n = 0;
  foreach ( Kigo k, kl )
    { if ( friendly )
        { if ( k.friendlyGoishi && !k.opponentGoishi && !k.emptyGrid ) // Do not count "don't care" grids
            n++;
        }
       else
        { if ( !k.friendlyGoishi && k.opponentGoishi && !k.emptyGrid ) // Do not count "don't care" grids
            n++;
        }
    }
  return n;
}

/**
 * @brief Ruikei::nCaptured
 * @param friendly - if true, count friendly captures, if false count opponent's
 * @return number of captures, either by friendly or opponent moves in the current Ruikei branch
 */
qint32  Ruikei::nCaptured( bool friendly )
{ qint32 fc = friendlyColor;
  qint32 n = friendly ? 0 : justCaptured; // last move was (always) by opponent
  Ruikei *pap = ap;
  while ( pap != nullptr )
    { if (( !friendly ) && ( fc == pap->friendlyColor )) // Is the pap Ruikei the result of an opponent's move?
        n += pap->justCaptured;
      if ((  friendly ) && ( fc != pap->friendlyColor ))
        n += pap->justCaptured;
      pap = pap->ap; // Work back through the branch
    }
  return n;
}

/**
 * @brief Ruikei::nTerritory
 * @param friendly - if true, count friendly territory, if false count opponent's
 * @return number of empty grid points completely surrounded by friendly or opponent Goishi
 */
qint32  Ruikei::nTerritory( bool friendly )
{ qint32 nt = 0;
  for ( qint32 i=0; i < nPoints(); i++ )
    if ( kl.at(i).isTerritory( friendly ) )
      nt++;
  return 0;
}

/**
 * @brief Ruikei::score - importantly, symmetric about 0 so negation converts a
 *   friendly score into an opponent score, and vice versa.
 * @return metric of the relative value of the Ruikei position for the friendly player
 */
qint32 Ruikei::score()
{ return nTerritory( true ) - nTerritory( false ) +
         nCaptured( true )  - nCaptured( false );
}

/**
 * @brief Ruikei::matchPosition
 * @param ap - Ruikei to try to match
 * @return true if the ap position is a match for this position, in any orientation
 */
bool Ruikei::matchPosition( Ruikei *ap )
{ // Try to fail fast before a full 8 orientation matchup test
  if (    (ap->rows + ap->columns) !=    (rows + columns)) return false;
  if ( abs(ap->rows - ap->columns) != abs(rows - columns)) return false;
  if (     ap->nGoishi( true  )    != nGoishi( true  )   ) return false;
  if (     ap->nGoishi( false )    != nGoishi( false )   ) return false;
  if (     ap->nEdges()            != nEdges()           ) return false;
  for ( orientation = 0; orientation < 8; orientation++ )
    { if ( ( Xsize()     == ap->Xsize()     ) &&
           ( x0Edge()    == ap->x0Edge()    ) &&
           ( xSizeEdge() == ap->xSizeEdge() ) &&
           ( Ysize()     == ap->Ysize()     ) &&
           ( y0Edge()    == ap->y0Edge()    ) &&
           ( ySizeEdge() == ap->ySizeEdge() ) )
        { bool allKigoMatch = true;
          for ( qint32 y = 0; (y < Ysize()) && allKigoMatch; y++ )
            for ( qint32 x = 0; (x < Xsize()) && allKigoMatch; x++ )
              if ( kigoAt( x,y ) != ap->kigoAt( x,y ) )
                allKigoMatch = false;
          if ( allKigoMatch )
            return true;
        }
    }
  return false;
}
