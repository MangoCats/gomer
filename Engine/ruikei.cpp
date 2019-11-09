#include "ruikei.h"

/**
 * @brief Ruikei::Ruikei - default constructor
 * @param xs - number of rows in this Ruikei
 * @param ys - number of columns in this Ruikei
 * @param p - parent Shiko
 */
Ruikei::Ruikei( qint32 xs, qint32 ys, Shiko *p ) : Menseki(xs,ys,p)
{ op = new Kogai(this);
  nGobanEdge =
  eGobanEdge =
  wGobanEdge =
  sGobanEdge = 0;
  friendlyColor = NO_PLAYER;
  kl.resize( nPoints() );
}

/**
 * @brief Ruikei::Ruikei - constructs the Ruikei from a data stream
 * @param ds - data stream to read the Ruikei in from
 * @param p - parent Shiko
 */
Ruikei::Ruikei( QDataStream &ds, Shiko *p ) : Menseki(p)
{ ds >> rows;
  ds >> columns;
  ds >> nGobanEdge;
  ds >> eGobanEdge;
  ds >> wGobanEdge;
  ds >> sGobanEdge;
  kl.resize( nPoints() );
  bool ok = true;
  for ( qint32 i = 0; i < nPoints(); ++i )
    ok &= kl[i].fromDataStream( ds );
  if ( !ok )
    qDebug( "Ruikei::Ruikei() problem reading Kigo list" );
   else
    op = new Kogai( ds, this );
  friendlyColor = NO_PLAYER;
  xo = yo = 0;
}

/**
 * @brief Ruikei::toDataStream
 * @param ds - DataStream to serialize this Ruikei to
 */
void Ruikei::toDataStream( QDataStream &ds ) const
{ ds << rows;
  ds << columns;
  ds << nGobanEdge;
  ds << eGobanEdge;
  ds << wGobanEdge;
  ds << sGobanEdge;
  for ( qint32 i = 0; i < nPoints(); ++i )
    kl.at(i).toDataStream( ds );
  if ( op != nullptr )
    op->toDataStream( ds );
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
 * @brief Ruikei::nEdges
 * @return the number of Goban edges this Ruikei touches 0-4
 */
qint32 Ruikei::nEdges() const
{ qint32 n = 0;
  if ( nGobanEdge ) ++n;
  if ( eGobanEdge ) ++n;
  if ( wGobanEdge ) ++n;
  if ( sGobanEdge ) ++n;
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
      case 6: return wGobanEdge;
      case 1:
      case 5: return nGobanEdge;
      case 2:
      case 4: return eGobanEdge;
      case 3:
      case 7: return sGobanEdge;
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
      case 4: return sGobanEdge;
      case 1:
      case 7: return wGobanEdge;
      case 2:
      case 6: return nGobanEdge;
      case 3:
      case 5: return eGobanEdge;
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
      case 6: return eGobanEdge;
      case 1:
      case 5: return sGobanEdge;
      case 2:
      case 4: return wGobanEdge;
      case 3:
      case 7: return nGobanEdge;
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
      case 4: return nGobanEdge;
      case 1:
      case 7: return eGobanEdge;
      case 2:
      case 6: return sGobanEdge;
      case 3:
      case 5: return wGobanEdge;
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

