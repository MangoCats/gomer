#include "territory.h"

Territory::Territory(Board *parent) : QObject(parent)
{ bp = parent;
  size = 0.4;
  show = false;
  for ( int i = 0; i < bp->Xsize; i++ )
    for ( int j = 0; j < bp->Ysize; j++ )
      gi.append( -1 );  // -1 == undetermined territory, not yet in a group
  update();
}

void  Territory::setScene( BoardScene *scp )
{ scene = scp; }

/**
 * @brief Territory::clear - erase existing territory objects
 *   in preparation for a new computation and possible display
 */
void Territory::clear()
{ clearDisplay();
  foreach( TerritorySquare *tp, squareList )
    delete tp;
  squareList.clear();
  foreach( TerritoryGroup *gp, groupList )
    delete gp;
  groupList.clear();
  for ( int i = 0; i < bp->Xsize; i++ )
    for ( int j = 0; j < bp->Ysize; j++ )
      gi.replace( i + j * bp->Xsize, -1 );
}

/**
 * @brief Territory::update - floodfill all empty grid points
 *   and determine their current territory status: black, white or undetermined.
 */
void Territory::update()
{ clear();
  // Determine how many territory groups there are, and what coordinates they each cover
  for ( int i = 0; i < bp->Xsize; i++ )
    for ( int j = 0; j < bp->Ysize; j++ )
      if ( gi.at( i + j * bp->Xsize ) == -1 )
        { tFill( i, j, groupList.size() );
          groupList.append( new TerritoryGroup(this) );
        }
  // Fill the territory groups with square objects
  for ( int k = 0; k < groupList.size(); k++ )
    { TerritoryGroup *tp = groupList.at(k);
      for ( int i = 0; i < bp->Xsize; i++ )
        for ( int j = 0; j < bp->Ysize; j++ )
          { if ( gi.at( i + j * bp->Xsize ) == k )
              { TerritorySquare *ts = new TerritorySquare(this);
                ts->x = i;
                ts->y = j;
                ts->g = k;
                ts->c = -2; // Undetermined, at this time
                squareList.append(ts);
                tp->group.append(ts);
              }
          }
    }
  // Determine the color of each territory group
  for ( int k = 0; k < groupList.size(); k++ )
    { TerritoryGroup *tp = groupList.at(k);
      int tc = -2;  // -2 undeterminmed, -1 determined to be disputed, 0 black, 1 white
      bool done = ( groupList.size() <= 0 ) || ( tp->group.size() == 0 );
      int i = 0;
      while( !done )
        { TerritorySquare *ts = tp->group.at(i);
          if ( ts->x > 0 )               done |= territoryColor( ts->x-1, ts->y, &tc );
          if ( ts->x < (bp->Xsize - 1) ) done |= territoryColor( ts->x+1, ts->y, &tc );
          if ( ts->y > 0 )               done |= territoryColor( ts->x, ts->y-1, &tc );
          if ( ts->y < (bp->Ysize - 1) ) done |= territoryColor( ts->x, ts->y+1, &tc );
          if ( ++i >= tp->group.size() ) done = true;
        }
      groupList.at(k)->c = tc;
      foreach ( TerritorySquare *ts, tp->group )
        ts->c = tc;
    }
  updateDisplay();
}

/**
 * @brief Territory::territoryColor - judging adjacent squares to a territory and the current color determination
 *   determine the continuing color evaluation of the territory
 * @param x - coordinate to evaluate
 * @param y - coordinate to evaluate
 * @param tc - current color assignment
 * @return true if the current color assignment has become disputed
 */
bool Territory::territoryColor( int x, int y, int *tc )
{ int ii = x + bp->Xsize * y;
  if ( bp->board.size() <= ii )
    return true;
  Stone *sp = bp->board.at( ii );
  if ( sp == nullptr )
    return false; // No info here
  if ( *tc == -1 )
    return true; // Done before we start
  if ( *tc == -2 )
    { *tc = sp->c;
      return false;
    }
  if ( *tc == sp->c )
    return false; // Continuing the trend
  *tc = -1;       // Contrasting neighbor found, territory is in dispute
  return true;
}

/**
 * @brief Territory::tFill - floodfill unclaimed territory with group index g
 * @param x - coordinate to look for unfilled neighbors from
 * @param y - coordinate to look for unfilled neighbors from
 * @param g - group index to fill this territory with
 */
void Territory::tFill( int x, int y, int g )
{ int ii = x + y * bp->Xsize;
  if ( bp->board.size() <= ii )
    return;
  if ( bp->board.at( ii ) != nullptr )
    { gi.replace( ii, -2 - bp->board.at( ii )->c );  // -2 for blackstone, -3 for whitestone
      return; // floodfill search does not pass a stone
    }
  gi.replace( ii, g );
  if ( x > 0 )                 tCheck( x-1, y, g );
  if ( x < ( bp->Xsize - 1 ) ) tCheck( x+1, y, g );
  if ( y > 0 )                 tCheck( x, y-1, g );
  if ( y < ( bp->Ysize - 1 ) ) tCheck( x, y+1, g );
}

/**
 * @brief Territory::tCheck
 * @param x - coordinate to check for stone
 * @param y - coordinate to check for stone
 * @param g - group index to mark this territory with
 */
void Territory::tCheck( int x, int y, int g )
{ int ii = x + y * bp->Xsize;
  if ( bp->board.size() <= ii )
    return; // Board is not ready yet, happens at first initialization
  if ( gi.size() < ii )
    { qDebug( "ERROR: gi not ready yet" );
      return;
    }
  if ( gi.at( ii ) != -1 )
    { if (( gi.at( ii ) > -1 ) && ( gi.at( ii ) != g ))
        { QString msg = QString( "ERROR: tCheck @(%1,%2,%3) encountered another territory index %4" ).arg(x).arg(y).arg(g).arg(gi.at( ii ) );
          qDebug( qPrintable( msg ) );
        }
      return; // already marked
    }
  if ( bp->board.at( ii ) != nullptr )
    { gi.replace( ii, -2 - bp->board.at( ii )->c );  // -2 for blackstone, -3 for whitestone
      return; // floodfill search does not pass a stone
    }
  tFill( x, y, g );  // Continue the floodfill search
}


/**
 * @brief Territory::clearDisplay - just clear the displayed
 *   squares' graphics items, not the underlying data objects.
 */
void Territory::clearDisplay()
{ if ( scene == nullptr )
    return;
  foreach( TerritorySquare *tp, squareList )
    { if ( tp->ri != nullptr )
        { tp->ri->setVisible( false );
          scene->removeMyItem( tp->ri );
          delete( tp->ri );
          tp->ri = nullptr;
        }
    }
}

/**
 * @brief Territory::updateDisplay - just update the display
 *   graphics items, not recalculating territory data objects.
 */
void Territory::updateDisplay()
{ clearDisplay();
  if ( !show )
    return;
  QPen   tPen;
  QBrush tBrush;
  foreach( TerritorySquare *tp, squareList )
    { // tp->ri == nullptr, done in clearDisplay
      if ( tp->c < 0 )
        { tPen   = scene->linePen;
          tBrush = scene->backBrush;
        }
       else if ( tp->c == 0 )
        { tPen   = Qt::NoPen;
          tBrush = scene->blackBrush;
        }
       else if ( tp->c == 1 )
        { tPen   = Qt::NoPen;
          tBrush = scene->whiteBrush;
        }
       else
        { tPen   = Qt::NoPen;
          tBrush = scene->backBrush;
          qDebug( "Territory::updateDisplay() unknown color" );
        }

      tp->ri = scene->addRect( (qreal)tp->x - size*0.5,
                               (qreal)tp->y - size*0.5,
                               size, size, tPen, tBrush ); // TODO: correct pen and brush for territory color
    }
}
