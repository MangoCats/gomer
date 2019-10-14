#include "goban.h"

Goban::Goban(Game *parent,qint32 xs,qint32 ys) : QObject(parent), gp(parent), Xsize(xs), Ysize(ys)
{ qDebug( "Goban constructor %d x %d", Xsize, Ysize );
  goishiChar = ".XO3456789";
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
  if ( grid.size() > nPoints() )
    grid.clear();
  while ( grid.size() < nPoints() )
    grid.append( nullptr );
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
        { qint32 c = grid.at(i)->color;
          if (( c > (goishiChar.size() - 2) ) || ( c < 0 ))
            s.append( "!" );
           else
            s.append( goishiChar.at(c+1) );
        }
    }
  return s;
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
 * @brief Goban::placeGoishiAt - place a Goishi on the Goban at the given coordinates
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
  return placeGoishi( ip, x + Xsize * y );
}

/**
 * @brief Goban::placeGoishiAt - place a Goishi on the Goban at the described coordinates
 * @param ip - pointer to Goishi to place
 * @param pos - string description of the coordinates
 * @return true if successful
 */
bool  Goban::placeGoishiAt( Goishi *ip, QString pos )
{ qint32 x = Xsize;
  if ( x > Xlabels.size() )
    { qDebug( "WARNING: Xsize larger than available labels." );
      return false;
    }
  bool done = false;
  while ( !done )
    { if ( x <= 0 )
        { qDebug( "Goban::placeGoishiAt(%s) no match found in Xlabels", qPrintable( pos ) );
          return false;
        }
      if ( pos.startsWith( Xlabels.at(--x) ) )
        done = true;
    }
  qint32 y = Ysize;
  if ( y > Ylabels.size() )
    { qDebug( "WARNING: Ysize larger than available labels." );
      return false;
    }
  done = false;
  while ( !done )
    { if ( y <= 0 )
        { qDebug( "Goban::placeGoishiAt(%s) no match found in Ylabels", qPrintable( pos ) );
          return false;
        }
      if ( pos.endsWith( Ylabels.at(--y) ) )
        done = true;
    }
  return placeGoishiAt( ip, x, y );
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
