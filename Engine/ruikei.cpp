#include "ruikei.h"

Ruikei::Ruikei( qint32 xs, qint32 ys, Shiko *p ) : Menseki(xs,ys,p)
{ op = new Kogai(this);
 // if ( testTransforms() )
 //   qDebug( "transform test passed." );
}

Ruikei::Ruikei( QDataStream &ds, Shiko *p ) : Menseki(p)
{ ds >> rows;
  ds >> columns;
  kl.resize( nPoints() );
  bool ok = true;
  for ( qint32 i = 0; i < nPoints(); i++ )
    { quint8 b;
      ds >> b;
        { ok &= kl[i].fromByte( b );
          i++;
        }
    }
  if ( !ok )
    qDebug( "Ruikei::Ruikei() problem reading Kigo list" );
  op = new Kogai( ds, this );
}

/**
 * @brief Ruikei::kigoList
 * @return the current Kigo list, serialized as a Byte array
 */
QByteArray  Ruikei::kigoList()
{ QByteArray ba;
  ba.append( (quint8)rows    );
  ba.append( (quint8)columns );
  qint32 i = 0;
  while ( i < kl.size() )
    ba.append( kl.at(i).toByte() );
  return ba;
}

/**
 * @brief fromByteArray - read in a kigoList that has been stored as a byte array
 * @param ba - byte array to de-serialize
 * @return true if successful
 */
bool Ruikei::fromByteArray( QByteArray ba )
{ qint32 xs,ys;
  if ( ba.size() < 11 )
    return false; // Minimum dimensions of 3
  xs = ba.at(0);
  ys = ba.at(1);
  if (( xs < 3 ) || ( ys < 3 ))
    return false;
  if ( ba.size() < (xs*ys + 2) )
    return false;
  rows = xs;
  columns = ys;
  orientation = 0; // Always stored in 0 orientation
  kl.resize( xs*ys );
  bool ok = true;
  qint32 i = 2;
  while ( i < ba.size() )
    { ok &= kl[i-2].fromByte( ba.at(i) );
      i++;
    }
  op->fromByteArray( ba.mid(xs*ys+2) );
  return ok;
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

