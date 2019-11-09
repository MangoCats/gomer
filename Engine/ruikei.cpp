#include "ruikei.h"

Ruikei::Ruikei( qint32 xs, qint32 ys, Shiko *p ) : Menseki(xs,ys,p)
{ op = new Kogai(this);
  nGobanEdge =
  eGobanEdge =
  wGobanEdge =
  sGobanEdge = 0;
 // if ( testTransforms() )
 //   qDebug( "transform test passed." );
}

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
    { quint8 b;
      ds >> b;
      ok &= kl[i].fromByte( b );
    }
  if ( !ok )
    qDebug( "Ruikei::Ruikei() problem reading Kigo list" );
   else
    op = new Kogai( ds, this );
}

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
{ return false;
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

