#include "goban.h"

Goban::Goban(Game *parent,qint32 xs,qint32 ys) : QObject(parent), gp(parent), Xsize(xs), Ysize(ys)
{ // qDebug( "Goban constructor %d x %d", Xsize, Ysize );
  goishiChar = ".,XO3456789";
  Xlabels.append("A");  Ylabels.append("1");
  Xlabels.append("B");  Ylabels.append("2");
  Xlabels.append("C");  Ylabels.append("3");
  Xlabels.append("D");  Ylabels.append("4");
  Xlabels.append("E");  Ylabels.append("5");
  Xlabels.append("F");  Ylabels.append("6");
  Xlabels.append("G");  Ylabels.append("7");
  Xlabels.append("H");  Ylabels.append("8");
  Xlabels.append("J");  Ylabels.append("9");
  Xlabels.append("K");  Ylabels.append("10");
  Xlabels.append("L");  Ylabels.append("11");
  Xlabels.append("M");  Ylabels.append("12");
  Xlabels.append("N");  Ylabels.append("13");
  Xlabels.append("O");  Ylabels.append("14");
  Xlabels.append("P");  Ylabels.append("15");
  Xlabels.append("Q");  Ylabels.append("16");
  Xlabels.append("R");  Ylabels.append("17");
  Xlabels.append("S");  Ylabels.append("18");
  Xlabels.append("T");  Ylabels.append("19");
  Xlabels.append("U");  Ylabels.append("20");
  Xlabels.append("V");  Ylabels.append("21");
  Xlabels.append("W");  Ylabels.append("22");
  Xlabels.append("X");  Ylabels.append("23");
  Xlabels.append("Y");  Ylabels.append("24");
  Xlabels.append("Z");  Ylabels.append("25");
  Xlabels.append("AA"); Ylabels.append("26");  // Incompatible with GTP, but Gomer can handle them.
  Xlabels.append("AB"); Ylabels.append("27");
  Xlabels.append("AC"); Ylabels.append("28");
  Xlabels.append("AD"); Ylabels.append("29");
  Xlabels.append("AE"); Ylabels.append("30");
  Xlabels.append("AF"); Ylabels.append("31");
  resize( xs, ys );
}

/**
 * @brief Goban::Goban - copy constructor
 * @param bp - Goban to copy
 * @param parent - parent of the new Goban
 */
Goban::Goban(Goban *bp, Game *parent) : QObject(parent), gp(parent), Xsize(bp->Xsize), Ysize(bp->Ysize)
{ Xdots      = bp->Xdots;
  Ydots      = bp->Ydots;
  goishiChar = bp->goishiChar;
  Xlabels    = bp->Xlabels;
  Ylabels    = bp->Ylabels;
  grid.reserve( nPoints() );
  for ( qint32 i = 0 ; i < nPoints() ; i++ )
    { Goishi *ip = bp->grid.at(i);
      if ( ip == nullptr )
        grid.append( nullptr );
       else
        grid.append( new Goishi(ip,this) );
    }
}

/**
 * @brief Goban::resize - board must be clear before a resize can succeed
 * @param xs - new size
 * @param ys - new size
 * @return true if successful
 */
bool Goban::resize( qint32 xs, qint32 ys )
{ if ( xs > Xlabels.size() )
    return false;
  if ( ys > Ylabels.size() )
    return false;
  foreach ( Goishi *ip, grid )
    if ( ip != nullptr )
      { qDebug( "ERROR: attempt to resize when board is not cleared." );
        return false;
      }
  Xsize = xs;
  Ysize = ys;
  grid.reserve( nPoints() );
  if ( grid.size() > nPoints() )
    grid.clear();
  while ( grid.size() < nPoints() )
    grid.append( nullptr );

  // For the ASCII board:
  Xdots.clear();
  Ydots.clear();
  Xdots.append( (Xsize < 13) ? 2 : 3 );
  Ydots.append( (Ysize < 13) ? 2 : 3 );
  Xdots.append( Xsize - ((Xsize < 13) ? 3 : 4) );
  Ydots.append( Ysize - ((Ysize < 13) ? 3 : 4) );
  if ( ( Xsize % 2 ) == 1 )
    Xdots.append( Xsize/2 );
  if ( ( Ysize % 2 ) == 1 )
    Ydots.append( Ysize/2 );
  return true;
}

/**
 * @brief Goban::state
 * @return a compact string representing all grid points and their occupying Goishi
 */
QString Goban::state()
{ QString s;
  for ( int i = 0 ; i < nPoints() ; i++ )
    { if ( grid.at(i) == nullptr )
        s.append( goishiChar.at(0) );
       else
        s.append( colorToChar( grid.at(i)->color ) );
    }
  return s;
}

/**
 * @brief Goban::colorToChar
 * @param c - color index
 * @return Character corresponding to the color index
 */
QChar Goban::colorToChar( qint32 c )
{ if (( c > (goishiChar.size() - 3) ) || ( c < 0 ))
    return QChar( '!' );
  return goishiChar.at(c+2);
}

/**
 * @brief Goban::takeGoishi - remove the Goishi (if any) at linear index i
 * @param i - linear index into the grid array
 * @return pointer to the Goishi that was at i, or nullptr if none was there
 */
Goishi *Goban::takeGoishi( qint32 i )
{ if (( i < 0 ) || ( i >= grid.size() ))
    { qDebug( "WARNING: Goban::takeGoishi(%d) out of range [0, %d)", i, grid.size() );
      return nullptr;
    }
  Goishi *ip = grid.at(i);
  if ( ip != nullptr )
    grid.replace( i, nullptr );
  emit boardChanged( state() );
  return ip;
}

/**
 * @brief Goban::removeGoishi - remove the passed Goishi from the board
 * @param ip - Goishi pointer
 */
void Goban::removeGoishi( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "WARNING: Goban::removeGoishi( nullptr )" );
      return;
    }
  qint32 i = xyToIndex( ip->x, ip->y );
  if (( i < 0 ) || ( i >= nPoints() ))
    { qDebug( "WARNING: Goban::removeGoishi() @ %d, %d not on board", ip->x, ip->y );
      return;
    }
  if ( grid.at(i) == nullptr )
    { qDebug( "WARNING: Goban::removeGoishi() no Goishi present @ %d, %d", ip->x, ip->y );
      return;
    }
  if ( grid.at(i) != ip )
    { qDebug( "ERROR: Goban::removeGoishi() different Goishi present @ %d, %d", ip->x, ip->y );
      return;
    }
  grid.replace( i, nullptr );
  emit boardChanged( state() );
}

/**
 * @brief Goban::placeGoishi - place a Goishi at linear index i
 * @param ip - pointer to Goishi to place
 * @param i - linear index into the array
 * @return true if successful
 */
bool Goban::placeGoishi( Goishi *ip, qint32 i )
{ if ( ip == nullptr )
    { qDebug( "WARNING: Goban::placeGoishi() will not place nullptr" );
      return false;
    }
  if (( i < 0 ) || ( i >= grid.size() ))
    { qDebug( "WARNING: Goban::placeGoishi() %d out of range [0, %d)", i, grid.size() );
      return false;
    }
  if ( grid.at(i) != nullptr )
    { qDebug( "WARNING: Goban::placeGoishi(%d) Goishi already placed there", i );
      return false;
    }
  grid.replace( i, ip );
  emit boardChanged( state() );
  return true;
}

/**
 * @brief Goban::placeGoishiAt - place a Goishi on the Goban at the given grid coordinates
 * @param ip - pointer to Goishi to place
 * @param x - coordinate to place at
 * @param y - coordinate to place at
 * @return true if successful
 */
bool Goban::placeGoishiAt( Goishi *ip, qint32 x, qint32 y )
{ if ( !onBoard(x,y) )
    { qDebug( "WARNING: Goban::placeGoishiAt(%d,%d) not on board",x,y );
      return false;
    }
  if ( goishiAt( x, y ) != nullptr )
    { qDebug( "WARNING: Goban::placeGoishiAt(%d,%d), Goishi already present there.",x,y );
      return false;
    }
  bool success = placeGoishi( ip, xyToIndex( x, y ) );
  if ( success )
    { ip->x = x;
      ip->y = y;
    }
  return success;
}

/**
 * @brief Goban::placeGoishiAt - place a Goishi on the Goban at the described coordinates
 * @param ip - pointer to Goishi to place
 * @param v - string description of the coordinates
 * @return true if successful
 */
bool  Goban::placeGoishiAt( Goishi *ip, QString v )
{ qint32 x,y;
  if ( !vertexToXY(v,&x,&y) )
    return false;
  return placeGoishiAt( ip, x, y );
}

/**
 * @brief Goban::vertexToXY
 * @param v - vertex as string
 * @param x - pointer to x coordinate
 * @param y - pointer to x coordinate
 * @return true if vertex conversion to xy was successful
 */
bool  Goban::vertexToXY( QString v, qint32 *x, qint32 *y )
{ v = v.toUpper();
  *x = Xsize;
  if ( *x > Xlabels.size() )
    { qDebug( "WARNING: Xsize larger than available labels." );
      return false;
    }
  bool done = false;
  while ( !done )
    { if ( *x <= 0 )
        { qDebug( "Goban::vertexToXY(%s) no match found in Xlabels", qPrintable( v ) );
          return false;
        }
      if ( v.startsWith( Xlabels.at(--*x) ) )
        done = true;
    }
  *y = Ysize;
  if ( *y > Ylabels.size() )
    { qDebug( "WARNING: Ysize larger than available labels." );
      return false;
    }
  done = false;
  while ( !done )
    { if ( *y <= 0 )
        { qDebug( "Goban::vertexToXY(%s) no match found in Ylabels", qPrintable( v ) );
          return false;
        }
      if ( v.endsWith( Ylabels.at(--*y) ) )
        done = true;
    }
  return true;
}

/**
 * @brief Goban::vertexToIndex
 * @param v - vertex string to translate
 * @return index, or -1 if vertex is invalid
 */
qint32  Goban::vertexToIndex( QString v )
{ qint32 x,y;
  if ( !vertexToXY( v, &x, &y ) )
    return -1;
  return xyToIndex( x, y );
}

/**
 * @brief Goban::indexToXY
 * @param i - index to convert
 * @param x - pointer to X coordinate variable
 * @param y - pointer to Y coordinate variable
 * @return true if successful
 */
bool  Goban::indexToXY( qint32 i, qint32 *x, qint32 *y )
{ if (( i < 0 ) || ( i >= nPoints() ))
    return false;
  *x = i % Xsize;
  *y = i / Xsize;
  return true;
}

/**
 * @brief indexToVertex
 * @param i - index to convert
 * @return vertex string, or empty if there is a problem
 */
QString  Goban::indexToVertex( qint32 i )
{ qint32 x,y;
  if ( !indexToXY( i, &x, &y ) )
    return "";
  return xyToVertex( x, y );
}

/**
 * @brief Goban::xyToVertex
 * @param x - coordinate to convert
 * @param y - coordinate to convert
 * @return vertex string, or empty if there is a problem
 */
QString  Goban::xyToVertex( qint32 x, qint32 y )
{ if (( x < 0 ) ||
      ( y < 0 ) ||
      ( x >= Xsize ) ||
      ( y >= Ysize ) ||
      ( x >= Xlabels.size() ) ||
      ( y >= Ylabels.size() ))
    return "";
  return Xlabels.at(x)+Ylabels.at(y);
}

/**
 * @brief Goban::goishiAt
 * @param x - coordinate to get Goishi pointer for
 * @param y - coordinate to get Goishi pointer for
 * @return pointer to the Goishi at x,y - nullptr if none
 */
Goishi *Goban::goishiAt( qint32 x, qint32 y )
{ if ( !onBoard(x,y) )
    { qDebug( "WARNING: Goban::goishiAt(%d,%d) not on board",x,y );
      return nullptr;
    }
  return grid.at( x + Xsize * y );
}

Goishi *Goban::goishiAt( QString v )
{ qint32 x = -1;
  qint32 y = -1;
  if ( vertexToXY( v, &x, &y ) )
    return goishiAt( x, y );
  return nullptr;
}

Goishi *Goban::goishi( qint32 i )
{ if (( i < 0 ) || ( i > nPoints() ))
    { qDebug( "WARNING: Goban::goishi(%d) not on board",i );
      return nullptr;
    }
  return grid.at( i );
}

/**
 * @brief Goban::onBoard
 * @param x
 * @param y
 * @return true if the coordinates are on the board
 */
bool  Goban::onBoard( qint32 x, qint32 y )
{ if ( x < 0 ) return false;
  if ( y < 0 ) return false;
  if ( x >= Xsize ) return false;
  if ( y >= Ysize ) return false;
  return true;
}

bool  Goban::onBoard( QString v )
{ if (( v == "pass" ) || ( v == "resign" ))
    return false;
  qint32 x,y;
  vertexToXY( v, &x, &y );
  return onBoard( x, y );
}


////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Goban::showBoard
 * @return ASCII art representation of the current board state
 */
QString Goban::showBoard()
{ QString bs,bl;
  bs.append( "\n"+xAxisLabels()+"\n" );
  qint32 y = Ysize - 1;
  while ( y >= 0 )
    { bl = centerString( Ylabels.at(y), -2 ) + " ";
      for ( qint32 x = 0 ; x < Xsize ; x++ )
        bl.append( asciiGoishi( x, y ) );
      bl.append( centerString( Ylabels.at(y), 2 ) );
      if ( y < gp->np )
        { bl.append( ( y == gp->pt ) ? " *" : "  " );
          bl.append( goishiChar.mid( y+2, 1 )+" "+QString::number( gp->spl.at(y)->lid.size() ) );
        }
      bs.append( bl  + "\n" );
      y--;
    }
  bs.append( xAxisLabels() );
  return bs;
}

/**
 * @brief Goban::xAxisLabels
 * @return the xAxis label string
 */
QString Goban::xAxisLabels()
{ QString bl = "   "; // space for Y labels
  qint32 i = 0;
  while ( i < Xsize )
    bl.append( centerString( Xlabels.at(i++), 2 ) );
  return bl;
}

/**
 * @brief Goban::centerString - for ASCII art work
 * @param s - string to center
 * @param len - number of characters in the final string
 * @return s centered in a string len characters long
 */
QString Goban::centerString( QString s, qint32 len )
{ QString ps;
  qint32 extraSpace = 0;
  if ( len < 0 )
    { len = -len;
      extraSpace = 1;
    }
  qint32 spaces = len - s.size() + extraSpace;
  if ( spaces < 0 )
    qDebug( "WARNING: s longer than len" );
  if ( spaces <= 0 )
    return s;
  while ( ps.size() < (spaces/2) )
    ps.append( " " );
  ps.append( s );
  while ( ps.size() < len )
    ps.append( " " );
  return ps;
}


/**
 * @brief Goban::asciiGoishi - simple representation of the board, can get fancier later.
 * @param x - grid point to represent
 * @param y - grid point to represent
 * @return X for black, O for white, . for empty, with leading and trailing spaces
 */
QString Goban::asciiGoishi( qint32 x, qint32 y )
{ Goishi *ip = goishiAt( x, y );
  if ( ip == nullptr )
    { if ( Xdots.contains(x) && Ydots.contains(y) )
        return centerString( goishiChar.at(1), 2 );
      return centerString( goishiChar.at(0), 2 );
    }
  qint32 c = ip->color;
  if (( c >= goishiChar.size() - 2 ) || ( c < 0 ))
    { qDebug( "WARNING: color out of range in asciiGoishi" );
      return "*";
    }
  return centerString( goishiChar.at( c+2 ), 2 );
}
