#include "ruikei.h"

Ruikei::Ruikei(QObject *parent) : Menseki(parent)
{// if ( testTransforms() )
 //   qDebug( "transform test passed." );

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

// Rotate 90 degrees CCW 3x, then mirror the original and do 3 more rotations
// Orientation 0: index starts at X(0)  ,Y(0)   and scans X+ first, then Y+
// Orientation 1: index starts at X(MAX),Y(0)   and scans Y+ first, then X-
// Orientation 2: index starts at X(MAX),Y(MAX) and scans X- first, then Y-
// Orientation 3: index starts at X(0)  ,Y(MAX) and scans Y- first, then X+
// Orientation 4: index starts at X(MAX),Y(0)   and scans X- first, then Y+
// Orientation 5: index starts at X(MAX),Y(MAX) and scans Y- first, then X-
// Orientation 6: index starts at X(0)  ,Y(MAX) and scans X+ first, then Y-
// Orientation 7: index starts at X(0)  ,Y(0)   and scans Y+ first, then X+

/**
 * @brief Ruikei::indexToXY
 * @param i - index to convert
 * @param x - rotated/mirrored x position corresponding with i
 * @param y - rotated/mirrored y position corresponding with i
 * @return true if successful
 */
bool Ruikei::indexToXY( qint32 i, qint32 *x, qint32 *y )
{ if ( (rows <= 0) || (columns <= 0) )
    { qDebug( "Ruikei::indexToXY() rows %d columns %d nonsensical.", rows, columns );
      return false;
    }
  if ( ( i < 0 ) || ( i >= nPoints() ) )
    { qDebug( "Ruikei::indexToXY() index %d out of range", i );
      return false;
    }
  switch ( orientation )
    { case 0: *y = i / Xsize(); *x = i - *y * Xsize();                                               return true;
      case 1: *x = i / Ysize(); *y = i - *x * Ysize(); *x = Xsize() - 1 - *x;                        return true;
      case 2: *y = i / Xsize(); *x = i - *y * Xsize(); *x = Xsize() - 1 - *x; *y = Ysize() - 1 - *y; return true;
      case 3: *x = i / Ysize(); *y = i - *x * Ysize();                        *y = Ysize() - 1 - *y; return true;
      case 4: *y = i / Xsize(); *x = i - *y * Xsize(); *x = Xsize() - 1 - *x;                        return true;
      case 5: *x = i / Ysize(); *y = i - *x * Ysize(); *x = Xsize() - 1 - *x; *y = Ysize() - 1 - *y; return true;
      case 6: *y = i / Xsize(); *x = i - *y * Xsize();                        *y = Ysize() - 1 - *y; return true;
      case 7: *x = i / Ysize(); *y = i - *x * Ysize();                                               return true;
    }
  qDebug( "unexpected orientation %d in Ruikei::indexToXY(%d)", orientation, i );
  return 0;
}

/**
 * @brief Ruikei::xyToIndex
 * @param x - coordinate on this Ruikei to translate
 * @param y - coordinate on this Ruikei to translate
 * @return -1 if off Ruikei, index for x,y otherwise
 */
qint32 Ruikei::xyToIndex( qint32 x, qint32 y )
{ if ( (rows <= 0) || (columns <= 0) )
    { qDebug( "Ruikei::xyToIndex() rows %d columns %d nonsensical.", rows, columns );
      return -1;
    }
  if ( ( x < 0 ) || ( x >= Xsize() ) ||
       ( y < 0 ) || ( y >= Ysize() ) )
    { qDebug( "Ruikei::xyToIndex(%d,%d) out of range %d,%d", x,y,Xsize(),Ysize() );
      return -1;
    }
  switch ( orientation )
    { case 0: return                x            +                y  * Xsize();
      case 1: return (Xsize() - 1 - x) * Ysize() +                y           ;
      case 2: return (Xsize() - 1 - x)           + (Ysize() - 1 - y) * Xsize();
      case 3: return                x  * Ysize() + (Ysize() - 1 - y)          ;
      case 4: return (Xsize() - 1 - x)           +                y  * Xsize();
      case 5: return (Xsize() - 1 - x) * Ysize() + (Ysize() - 1 - y)          ;
      case 6: return                x            + (Ysize() - 1 - y) * Xsize();
      case 7: return                x  * Ysize() +                y           ;
    }
  qDebug( "unexpected orientation %d in Ruikei::xyToIndex(%d,%d)", orientation, x, y );
  return -1;
}

/**
 * @brief Ruikei::Xsize
 * @return number of columns in the rotated Ruikei
 */
qint32 Ruikei::Xsize()
{ switch ( orientation )
    { case 0:
      case 2:
      case 4:
      case 6: return columns;
      case 1:
      case 3:
      case 5:
      case 7: return rows;
    }
  qDebug( "unexpected orientation %d in Ruikei::Xsize()", orientation );
  return 0;
}

/**
 * @brief Ruikei::Ysize
 * @return number of rows in the rotated Ruikei
 */
qint32 Ruikei::Ysize()
{ switch ( orientation )
    { case 0:
      case 2:
      case 4:
      case 6: return rows;
      case 1:
      case 3:
      case 5:
      case 7: return columns;
    }
  qDebug( "unexpected orientation %d in Ruikei::Ysize()", orientation );
  return 0;
}

/**
 * @brief Ruikei::testTransforms - check xyToIndex and indexToXY transforms against each other
 * @return true if all transforms return identical
 */
bool Ruikei::testTransforms()
{ rows    = 2;
  columns = 3;
  qint32 o,i,x,y,ti,tx,ty,r,c;
  for ( r = 2; r < 20; r++ )
    for ( c = 2; c < 20; c++ )
      for ( o = 0; o < 8; o++ )
        { rows = r;
          columns = c;
          orientation = o;
          for ( x = 0; x < Xsize(); x++ )
            for ( y = 0; y < Ysize(); y++ )
              { i = xyToIndex(x,y);
                indexToXY(i,&tx,&ty);
                if (( tx != x ) || ( ty != y ))
                  { qDebug( "Ruikei::testTransforms() fail @ %d %d %d > %d,%d to %d to %d,%d",r,c,o,x,y,i,tx,ty );
                    return false;
                  }
              }
          for ( i = 0; i < nPoints(); i++ )
            { indexToXY(i,&x,&y);
              ti = xyToIndex(x,y);
              if ( ti != i )
                { qDebug( "Ruikei::testTransforms() fail @ %d %d %d > %d to %d,%d to %d",r,c,o,i,x,y,ti );
                  return false;
                }
            }
        }
  return true;
}
