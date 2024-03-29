#include "menseki.h"

Menseki::Menseki( qint32 xs, qint32 ys, Dotei *p ) : QObject(p), vp(p)
{ rows = xs;
  columns = ys;
  init();
}

Menseki::Menseki( Dotei *p ) : QObject(p), vp(p)
{ rows = columns = -1;
  init();
}

void Menseki::init()
{ orientation = 0;
}

/**
 * @brief Menseki::colorToChar
 * @param c - color index
 * @return Character corresponding to the color index
 */
QChar Menseki::colorToChar( qint32 c ) const
{ if (( c > ( vp->goishiChar.size() - 3) ) || ( c < 0 ))
    return QChar( '!' );
  return vp->goishiChar.at(c+2);
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
 * @brief Menseki::indexToXY
 * @param i - index to convert
 * @param x - rotated/mirrored x position corresponding with i
 * @param y - rotated/mirrored y position corresponding with i
 * @return true if successful
 */
bool Menseki::indexToXY( qint32 i, qint32 *x, qint32 *y ) const
{ if ( (rows <= 0) || (columns <= 0) )
    { qDebug( "Menseki::indexToXY() rows %d columns %d nonsensical.", rows, columns );
      return false;
    }
  if ( ( i < 0 ) || ( i >= nPoints() ) )
    { qDebug( "Menseki::indexToXY() index %d out of range", i );
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
  qDebug( "unexpected orientation %d in Menseki::indexToXY(%d)", orientation, i );
  return 0;
}

/**
 * @brief Menseki::xyToIndex
 * @param x - coordinate on this Menseki to translate
 * @param y - coordinate on this Menseki to translate
 * @return -1 if off Menseki, index for x,y otherwise
 */
qint32 Menseki::xyToIndex( qint32 x, qint32 y ) const
{ if ( (rows <= 0) || (columns <= 0) )
    { qDebug( "Menseki::xyToIndex() rows %d columns %d nonsensical.", rows, columns );
      return -1;
    }
  if ( ( x < 0 ) || ( x >= Xsize() ) ||
       ( y < 0 ) || ( y >= Ysize() ) )
    { qDebug( "Menseki::xyToIndex(%d,%d) out of range %d,%d", x,y,Xsize(),Ysize() );
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
  qDebug( "unexpected orientation %d in Menseki::xyToIndex(%d,%d)", orientation, x, y );
  return -1;
}

/**
 * @brief Menseki::indexNeighbors
 * @param i - grid index to compare
 * @param j - grid index to compare
 * @return true if i is "next to" j on the grid
 */
bool  Menseki::indexNeighbors( qint32 i, qint32 j ) const
{ qint32 xi,yi,xj,yj;
  indexToXY(i,&xi,&yi);
  indexToXY(j,&xj,&yj);
  if ( xi > 0 )           if ( ( (xi-1) == xj ) && ( yi == yj ) ) return true;
  if ( xi < Xsize() - 1 ) if ( ( (xi+1) == xj ) && ( yi == yj ) ) return true;
  if ( yi > 0 )           if ( ( xi == xj ) && ( (yi-1) == yj ) ) return true;
  if ( yi < Ysize() - 1 ) if ( ( xi == xj ) && ( (yi+1) == yj ) ) return true;
  return false;
}

/**
 * @brief Menseki::vertexToXY
 * @param v - vertex as string
 * @param x - pointer to x coordinate
 * @param y - pointer to x coordinate
 * @return true if vertex conversion to xy was successful
 */
bool  Menseki::vertexToXY( QString v, qint32 *x, qint32 *y ) const
{ v = v.toUpper();
  *x = Xsize();
  if ( *x > vp->Xlabels.size() )
    { qDebug( "WARNING: Xsize larger than available labels." );
      return false;
    }
  bool done = false;
  while ( !done )
    { if ( *x <= 0 )
        { qDebug( "Menseki::vertexToXY(%s) no match found in Xlabels", qPrintable( v ) );
          return false;
        }
      if ( v.startsWith( vp->Xlabels.at(--*x) ) )
        done = true;
    }
  *y = Ysize();
  if ( *y > vp->Ylabels.size() )
    { qDebug( "WARNING: Ysize larger than available labels." );
      return false;
    }
  done = false;
  while ( !done )
    { if ( *y <= 0 )
        { qDebug( "Menseki::vertexToXY(%s) no match found in Ylabels", qPrintable( v ) );
          return false;
        }
      if ( v.endsWith( vp->Ylabels.at(--*y) ) )
        done = true;
    }
  return true;
}

/**
 * @brief Menseki::vertexToIndex
 * @param v - vertex string to translate
 * @return index, or -1 if vertex is invalid
 */
qint32  Menseki::vertexToIndex( QString v ) const
{ qint32 x,y;
  if ( !vertexToXY( v, &x, &y ) )
    return -1;
  return xyToIndex( x, y );
}

/**
 * @brief indexToVertex
 * @param i - index to convert
 * @return vertex string, or empty if there is a problem
 */
QString  Menseki::indexToVertex( qint32 i ) const
{ qint32 x,y;
  if ( !indexToXY( i, &x, &y ) )
    return "";
  return xyToVertex( x, y );
}

/**
 * @brief Menseki::xyToVertex
 * @param x - coordinate to convert
 * @param y - coordinate to convert
 * @return vertex string, or empty if there is a problem
 */
QString  Menseki::xyToVertex( qint32 x, qint32 y ) const
{ if (( x < 0 ) ||
      ( y < 0 ) ||
      ( x >= Xsize() ) ||
      ( y >= Ysize() ) ||
      ( x >= vp->Xlabels.size() ) ||
      ( y >= vp->Ylabels.size() ))
    return "";
  return vp->Xlabels.at(x)+vp->Ylabels.at(y);
}


/**
 * @brief Menseki::Xsize
 * @return number of columns in the rotated Menseki
 */
qint32 Menseki::Xsize() const
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
  qDebug( "unexpected orientation %d in Menseki::Xsize()", orientation );
  return 0;
}

/**
 * @brief Menseki::Ysize
 * @return number of rows in the rotated Menseki
 */
qint32 Menseki::Ysize() const
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
  qDebug( "unexpected orientation %d in Menseki::Ysize()", orientation );
  return 0;
}

/**
 * @brief Menseki::testTransforms - check xyToIndex and indexToXY transforms against each other
 * @return true if all transforms return identical
 */
bool Menseki::testTransforms()
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
                  { qDebug( "Menseki::testTransforms() fail @ %d %d %d > %d,%d to %d to %d,%d",r,c,o,x,y,i,tx,ty );
                    return false;
                  }
              }
          for ( i = 0; i < nPoints(); i++ )
            { indexToXY(i,&x,&y);
              ti = xyToIndex(x,y);
              if ( ti != i )
                { qDebug( "Menseki::testTransforms() fail @ %d %d %d > %d to %d,%d to %d",r,c,o,i,x,y,ti );
                  return false;
                }
            }
        }
  return true;
}
