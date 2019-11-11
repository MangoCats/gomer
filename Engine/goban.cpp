#include "goban.h"

Goban::Goban(Shiai *p,qint32 xs,qint32 ys) : Menseki(xs,ys,&(p->v)), gp(p)
{ // qDebug( "Goban constructor %d x %d", Xsize, Ysize );
  resize( xs, ys );
}

/**
 * @brief Goban::Goban - copy constructor
 * @param bp - Goban to copy
 * @param p - parent of the new Goban
 */
Goban::Goban(Goban *bp, Shiai *p) : Menseki(bp->rows,bp->columns,&(p->v)), gp(p)
{ Xdots = bp->Xdots;
  Ydots = bp->Ydots;
  grid.reserve( nPoints() );
  for ( qint32 i = 0 ; i < nPoints() ; i++ )
    grid.append( nullptr );
  for ( qint32 i = 0; i < bp->nPoints(); i++ )
    { Goishi *ip = bp->grid.at(i);
      if ( ip != nullptr )
        placeGoishi( new Goishi( ip, this ), i );
    }
  // Wyrms will be filled during Shiko copy
}

/**
 * @brief Goban::resize - board must be clear before a resize can succeed
 * @param xs - new size
 * @param ys - new size
 * @return true if successful
 */
bool Goban::resize( qint32 xs, qint32 ys )
{ if ( xs > vp->Xlabels.size() )
    return false;
  if ( ys > vp->Ylabels.size() )
    return false;
  foreach ( Goishi *ip, grid )
    if ( ip != nullptr )
      { qDebug( "ERROR: attempt to resize when board is not cleared." );
        return false;
      }
  rows    = xs;
  columns = ys;
  grid.reserve( nPoints() );
  if ( grid.size() > nPoints() )
    grid.clear();
  while ( grid.size() < nPoints() )
    grid.append( nullptr );

  // For the ASCII board:
  Xdots.clear();
  Ydots.clear();
  Xdots.append( (Xsize() < 13) ? 2 : 3 );
  Ydots.append( (Ysize() < 13) ? 2 : 3 );
  Xdots.append( Xsize() - ((Xsize() < 13) ? 3 : 4) );
  Ydots.append( Ysize() - ((Ysize() < 13) ? 3 : 4) );
  if ( ( Xsize() % 2 ) == 1 )
    Xdots.append( Xsize()/2 );
  if ( ( Ysize() % 2 ) == 1 )
    Ydots.append( Ysize()/2 );
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
        s.append( vp->goishiChar.at(0) );
       else
        s.append( colorToChar( grid.at(i)->color ) );
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
 * @brief Goban::removeGoishi - remove the passed Goishi from the board
 * @param ip - Goishi pointer
 */
void Goban::removeGoishi( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "WARNING: Goban::removeGoishi( nullptr )" );
      return;
    }
  if ( ip->bp != this )
    { qDebug( "WARNING: Goban::removeGoishi() Goishi Goban pointer mismatch" );
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
  qint32 x,y;
  indexToXY( i,&x,&y );
  ip->x = x;
  ip->y = y;
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
  return grid.at( x + Xsize() * y );
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

qint32 Goban::color( qint32 i )
{ Goishi *ip = goishi(i);
  if ( ip == nullptr )
    return NO_PLAYER;
  return ip->color;
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
  if ( x >= Xsize() ) return false;
  if ( y >= Ysize() ) return false;
  return true;
}

bool  Goban::onBoard( QString v )
{ if (( v == "pass" ) || ( v == "resign" ))
    return false;
  qint32 x,y;
  vertexToXY( v, &x, &y );
  return onBoard( x, y );
}

/**
 * @brief Goban::onEdge
 * @param i - indexed board position to check
 * @return true if the indexed position is on an edge of this Goban
 */
bool  Goban::onEdge( qint32 i ) const
{ qint32 x,y;
  indexToXY( i, &x, &y );
  if ( x == 0 ) return true;
  if ( y == 0 ) return true;
  if ( x == Xsize() - 1 ) return true;
  if ( y == Ysize() - 1 ) return true;
  return false;
}

/**
 * @brief Goban::chihoOnEdge
 * @param hp - Chiho to check
 * @return true if any element of this Chiho is on an edge of this Goban
 */
bool  Goban::chihoOnEdge( Chiho *hp ) const
{ foreach ( qint32 i, hp->bi )
    if ( onEdge(i) )
      return true;
  return false;
}

/**
 * @brief Goban::chihoXYsize
 * @param hp - Chiho to scan
 * @param xs - x size of Chiho on this Goban
 * @param ys - y size of Chiho on this Goban
 */
void  Goban::chihoXYsize( Chiho *hp, qint32 &xs, qint32 &ys ) const
{ qint32 minX,maxX,minY,maxY;
  chihoXYlimits( hp, minX, minY, maxX, maxY );
  xs = maxX - minX + 1; if ( xs < 0 ) xs = 0; if ( xs > Xsize() ) xs = Xsize();
  ys = maxY - minY + 1; if ( ys < 0 ) ys = 0; if ( ys > Ysize() ) ys = Ysize();
}

/**
 * @brief Goban::chihoXYlimits
 * @param hp - Chiho to scan
 * @param minX - limit of Chiho on this Goban
 * @param minY - limit of Chiho on this Goban
 * @param maxX - limit of Chiho on this Goban
 * @param maxY - limit of Chiho on this Goban
 */
void  Goban::chihoXYlimits( Chiho *hp, qint32 &minX, qint32 &minY, qint32 &maxX, qint32 &maxY ) const
{ minX = Xsize();
  maxX = 0;
  minY = Ysize();
  maxY = 0;
  qint32 x,y;
  foreach ( qint32 i, hp->bi )
    { indexToXY( i, &x, &y );
      if ( x < minX ) minX = x;
      if ( x > maxX ) maxX = x;
      if ( y < minY ) minY = y;
      if ( y > maxY ) maxY = y;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Goban::showBoard
 * @return ASCII art representation of the current board state
 */
QString Goban::showBoard()
{ QString bs,bl;
  bs.append( "\n"+xAxisLabels()+"\n" );
  qint32 y = Ysize() - 1;
  while ( y >= 0 )
    { bl = centerString( vp->Ylabels.at(y), -2 ) + " ";
      for ( qint32 x = 0 ; x < Xsize() ; x++ )
        bl.append( asciiGoishi( x, y ) );
      bl.append( centerString( vp->Ylabels.at(y), 2 ) );
      if ( y < gp->np )
        { bl.append( ( y == gp->pt ) ? " *" : "  " );
          bl.append( vp->goishiChar.mid( y+2, 1 )+" "+QString::number( gp->spl.at(y)->lid.size() ) );
        }
      bs.append( bl  + "\n" );
      y--;
    }
  bs.append( xAxisLabels() );
  return bs;
}


QString Goban::showChiho( Chiho *hp )
{ QString s;
  foreach ( qint32 i, hp->bi )
    s.append( indexToVertex(i)+" " );
  s.chop(1);
  return s + "\n";
}


/**
 * @brief Goban::xAxisLabels
 * @return the xAxis label string
 */
QString Goban::xAxisLabels()
{ QString bl = "   "; // space for Y labels
  qint32 i = 0;
  while ( i < Xsize() )
    bl.append( centerString( vp->Xlabels.at(i++), 2 ) );
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
        return centerString( vp->goishiChar.at(1), 2 );
      return centerString( vp->goishiChar.at(0), 2 );
    }
  qint32 c = ip->color;
  if (( c >= vp->goishiChar.size() - 2 ) || ( c < 0 ))
    { qDebug( "WARNING: color out of range in asciiGoishi" );
      return "*";
    }
  return centerString( vp->goishiChar.at( c+2 ), 2 );
}

////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Goban::fillByRules
 * @param c - color to test while filling
 * @param rule - to fill by vs color, true = match, false = does not match
 * @return list of all Chiho for the current Goban state which match c-rule
 */
QList<Chiho *> Goban::fillByRule( qint32 c, bool rule )
{ // qDebug( "Goban::fillByRule(%d,%d)",c,rule );
  QList<Chiho *> hpl;
  Chiho *hp = nullptr;
  qint32 x,y;
  for ( qint32 i = 0; i < nPoints(); i++ ) // Fill from every possible starting point
    { if ( hp == nullptr )
        hp = new Chiho( this );
      if ( !chihoListContains( hpl, i ) )
        { indexToXY( i,&x,&y );
          if ( fill( x, y, c, rule, hp ) )
            { hpl.append( hp );
              hp = nullptr;
            }
        }
    }
  if ( hp != nullptr )
    hp->deleteLater();
  // qDebug( "Goban::fillByRule(%d,%d) list size %d",c,rule,hpl.size() );
  return hpl;
}

/**
 * @brief Goban::chihoListContains
 * @param hpl - Chiho list to search
 * @param i - index to search for
 * @return true if i appears in any Chiho in hpl
 */
bool Goban::chihoListContains( QList<Chiho *> hpl, qint32 i )
{ foreach ( Chiho *hp, hpl )
    if ( hp != nullptr )
      if ( hp->contains(i) )
        return true;
  return false;
}


/**
 * @brief Goban::fill
 * @param x - coordinate to check
 * @param y - coordinate to check
 * @param c  - color-state of the grid points to fill, NO_PLAYER means no Goishi present
 * @param rule - should the fill area be == c (true) or != c (false)
 * @param hp - Chiho that is recording this floodfill result
 * @return true if x,y matches the c-rule
 */
bool Goban::fill( qint32 x, qint32 y, qint32 c, bool rule, Chiho *hp )
{ if ( !fillRuleCheck(x,y,c,rule,hp) )
    return false; // Done with this branch of the search
  // This gridpoint matches the rules, save it
  hp->addGobanIndex( xyToIndex(x,y) );
  // And search the neighbors
  if ( x > 0 )           fill( x-1,y,c,rule,hp );
  if ( x < Xsize() - 1 ) fill( x+1,y,c,rule,hp );
  if ( y > 0 )           fill( x,y-1,c,rule,hp );
  if ( y < Ysize() - 1 ) fill( x,y+1,c,rule,hp );
  return true; // x,y met the c-rule condition
}

/**
 * @brief Goban::fillRuleCheck
 * @param x - coordinate to check
 * @param y - coordinate to check
 * @param c  - color-state of the grid points to fill, NO_PLAYER means no Goishi present
 * @param rule - should the fill area be == c (true) or != c (false)
 * @param hp - Chiho that is recording this floodfill result
 * @return true if the c-rule is met at x,y
 */
bool Goban::fillRuleCheck( qint32 x, qint32 y, qint32 c, bool rule, Chiho *hp )
{ qint32 i = xyToIndex(x,y);
  if ( hp->contains(i) )
    return false; // Already been here
  if ( c == color(i) )
    return rule;  // Looking for c, and found it
  return !rule;   // Didn't find c
}
