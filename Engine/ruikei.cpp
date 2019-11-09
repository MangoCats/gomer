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
 // if ( testTransforms() )
 //   qDebug( "transform test passed." );
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
bool  Ruikei::isValid()
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
 * @brief Ruikei::matchOne
 * @param wp - Wyrm to try to match to a single Wyrm pattern
 * @param bp - Goban the Wyrm resides on, must also match pattern
 * @return true if a match is
 */
bool Ruikei::matchOne( Wyrm *wp, Goban *bp )
{ for ( qint32 i = 0; i < 7; i++ )
    { orientation = i;
      if ( matchOneInCurrentOrientation( wp, bp ) )
        return true;
    }
  return false;
}

bool Ruikei::matchOneInCurrentOrientation( Wyrm *wp, Goban *bp )
{ qint32 ne = nEdges();
  if ( ne == 4 )
    { if (( bp->Xsize() != Xsize() ) ||
          ( bp->Ysize() != Ysize() ))
        return false;
    }

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

