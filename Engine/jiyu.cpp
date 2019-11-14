#include "jiyu.h"

/**
 * @brief Jiyu::Jiyu - constructor
 * @param p - Goban parent of this Jiyu
 */
Jiyu::Jiyu(Goban *p) : QObject(p), bp(p)
{ clear(); }

/**
 * @brief Jiyu::Jiyu - copy constructor
 * @param jp - Jiyu to copy
 * @param p - parent of the new Jiyu
 */
Jiyu::Jiyu( Jiyu *jp, Goban *p ) : QObject(p), bp(p)
{ grid = jp->grid; }


/**
 * @brief Jiyu::clear - reset the grid to initial state
 */
void Jiyu::clear()
{ if ( bp == nullptr )
    { grid.clear();
      return;
    }
  qint32 npts = bp->nPoints();
  grid.reserve( npts );
  grid.clear();
  while ( grid.size() < npts )
    grid.append( QPair<qint32,qint32>(NO_PLAYER,npts) );  // Color, level - higher levels are weaker.
  if ( bp->gp == nullptr )
    { points.clear();
      return;
    }
  qint32 np = bp->gp->np; // Number of players
  points.reserve( np );
  points.clear();
  while ( points.size() < np )
    points.append( 0.0 );
}

/**
 * @brief Jiyu::update - recalculate grid and points
 */
void Jiyu::update()
{ clear();
  if ( bp == nullptr )
    { qDebug( "Jiyu::update Goban null" );
      return;
    }
  qint32 npts  = bp->nPoints();
  qint32 npr   = npts;  // Number of points remaining to classify
  qint32 level = 0;
  for ( qint32 i = 0; i < npts; i++ ) // Read in the Goishi
    { qint32 c = bp->color(i);
      if ( c != NO_PLAYER )
        { grid[i] = QPair<qint32,qint32>(c,level); // Goishi level is 0
          npr--;
        }
    }
  // Flood out with increasing levels until all points are covered
  qint32 x,y;
  while ( npr > 0 )
    { for ( qint32 i = 0; i < npts; i++ )
        { bp->indexToXY(i,&x,&y);
          if ( x > 0 )               if ( check( i,x-1,y,level ) ) npr--;
          if ( x < bp->Xsize() - 1 ) if ( check( i,x+1,y,level ) ) npr--;
          if ( y > 0 )               if ( check( i,x,y-1,level ) ) npr--;
          if ( y < bp->Ysize() - 1 ) if ( check( i,x,y+1,level ) ) npr--;
        }
      level++;
    }
  // Extra credit for "owning" edges
  for ( x = 0; x < bp->Xsize() - 1; x++ )
    { edgeBoost( x, 0 );
      edgeBoost( x, bp->Ysize() - 1 );
    }
  for ( y = 0; y < bp->Ysize() - 1; y++ )
    { edgeBoost( 0, y );
      edgeBoost( bp->Xsize() - 1, y );
    }

  updateScore();
}

/**
 * @brief Jiyu::check
 * @param i - grid index to update
 * @param x - neighbor coordinate to check
 * @param y - neighbor coordinate to check
 * @param level - level update is working at
 */
bool Jiyu::check( qint32 i, qint32 x, qint32 y, qint32 level )
{ qint32 in = bp->xyToIndex(x,y);    // Index of the neighbor point
  if ( grid.at(in).second != level ) // Only expanding from "level" points
    return false;
  level++;                           // Updating to level+1, if appropriate
  if ( grid.at(i).second < level )   // Cannot flood into lower levels
    return false;
  qint32 cn = grid.at(in).first;     // Color of the neighbor
  qint32 c  = grid.at(i).first;      // Color of the point being updated
  if ( grid.at(i).second == level )  // Already at level
    { if ( c != cn )                 // Is there a color mismatch?
        grid[i].first = NO_PLAYER; // Yes, so the color is reset to no owner.
      return false;
    }
  grid[i] = QPair<qint32,qint32>(cn,level); // New marking, owned by the neighbor's color
  return true;
}

/**
 * @brief Jiyu::edgeBoost - give "extra credit" to territory on the board edge
 * @param x - coordinate to give a level boost to
 * @param y - coordinate to give a level boost to
 */
void Jiyu::edgeBoost( qint32 x, qint32 y )
{ qint32 i = bp->xyToIndex(x,y);
  if ( grid.at(i).second > 1 ) // No edgeBoost for Goishi on the "losing line"
    grid[i].second = grid.at(i).second - 1;
}

/**
 * @brief Jiyu::updateScore - calculate points for each player
 *   always runs after a clear() so points are pre-initialized to 0.0
 */
void Jiyu::updateScore()
{ if ( bp     == nullptr )
    { qDebug( "Jiyu::updateScore Goban null" );
      return;
    }
  if ( bp->gp == nullptr )
    { qDebug( "Jiyu::updateScore Game null" );
      return;
    }
  qint32 np = bp->gp->np;
  QPair<qint32,qint32> jg;
  foreach ( jg , grid )
    { if ( jg.first != NO_PLAYER )
        { if (( jg.first >= np ) || ( jg.first < 0 ))
            qDebug( "Jiyu::score() unexpected color %d level %d", jg.first, jg.second );
           else
            { qreal pgp; // Points for this grid point
              if ( jg.second == 0 )
                pgp = 1.0; // 1.0 point on each Goishi
               else
                pgp = 1.0 / (4.0*((qreal)(jg.second * jg.second))); // 1.0 point total for the 4 1st liberties, 0.5 points total for the 8 2nd liberties, 1/3 point total for the 12 3rd liberties, etc.
              points[jg.first] += pgp;
    }   }   }
}

/**
 * @brief Jiyu::score
 * @param c - color of player to get score for
 * @return player's score, relative to all other players
 */
qreal Jiyu::score( qint32 c )
{ if ( bp     == nullptr )
    { qDebug( "Jiyu::score %d Goban null",c );
      return 0.0;
    }
  if ( bp->gp == nullptr )
    { qDebug( "Jiyu::score %d Game null",c );
      return 0.0;
    }
  qint32 np = bp->gp->np;
  if (( c < 0 ) || ( c >= np )) return 0.0;
  qreal pts = points.at(c);
  for ( qint32 pl = 0; pl < np; pl++ )
    if ( pl != c )
      pts -= points.at(pl);
  return pts;
}

/**
 * @brief Jiyu::show
 * @return ASCII output of Jiyu contents
 */
QString Jiyu::show()
{ QString level = "0123456789ABCDEFGHJKLMNOPQRSTUVWXYZ";
  if ( bp == nullptr ) return "Goban null, Jiyu will not show()";
  QString msg = "\n";
  for ( qint32 y = bp->Ysize() - 1; y >= 0; y-- )
    { for ( qint32 x = 0; x < bp->Xsize(); x++ )
        { qint32 i = bp->xyToIndex(x,y);
          QChar cc = QChar('.');
          if ( grid.at(i).first != NO_PLAYER )
            cc = bp->colorToChar( grid.at(i).first );
          msg.append( QString( "%1%2" ).arg( cc )
                        .arg(level.mid(grid.at(i).second,1)) );
        }
      msg.append( "\n" );
    }
  if ( bp->gp != nullptr )
    for ( qint32 pl = 0; pl < bp->gp->np; pl++ )
      msg.append( QString( "%1 %2\n" ).arg( bp->colorToChar(pl) ).arg( points.at(pl) ) );
  return msg+"\n";
}
