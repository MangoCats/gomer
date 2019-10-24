#include "chiiki.h"

/**
 * @brief Chiiki::Chiiki - basic constructor, whole Goban analysis
 *   of territory
 * @param p - parent Shiko
 */
Chiiki::Chiiki(Shiko *p) : QObject(p), bp(p->bp), tp(p)
{ resizeGoban();
}

/**
 * @brief Chiiki::Chiiki - copy constructor
 * @param pcp - pointer to the Chiiki this is making a copy of
 * @param p - Shiko parent of the new Chiiki
 */
Chiiki::Chiiki( Chiiki *pcp, Shiko *p ) : QObject(p), bp(pcp->bp), tp(p)
{ if ( tp == nullptr )     { qDebug( "WARNING: Chiiki::resizeGoban() Shiko null" ); return; }
  if ( tp->gp == nullptr ) { qDebug( "WARNING: Chiiki::resizeGoban() Game null" ); return; }
  qint32 np = tp->gp->np;          // Number of Players in this Game
  resizeGoban();                   // Size the local data structures to match the currrent Goban
  for ( qint32 pl = 0; pl <= np; pl++ )
    { foreach ( Ryoiki *rp, pcp->rpm.at(pl) )
        rpm[pl].append( new Ryoiki( rp, this ) );
      foreach ( Ryoiki *rp, rpm.at(pl) )
        foreach ( qint32 i, rp->bi )
          rGrid[pl].replace( i, rp );
    } // for pl
}

QString  Chiiki::showRyoiki()
{ QString s;
  for ( qint32 pl = 0; pl <= tp->gp->np; pl++ )
    s += QString( "Ryoiki %1:%2\n" ).arg(pl).arg(showRyoiki(pl) );
  return s;
}

QString Chiiki::showRyoiki( qint32 c )
{ QString s;
  foreach ( Ryoiki *rp, rpm.at(c) )
    s.append( bp->showChiho( rp ) );
  return "\n" + s;
}

/**
 * @brief Chiiki::resizeGoban - also used for initial allocation
 *   make sure the rGrid has at least as many entries as the
 *   current Goban has grid points.
 */
void Chiiki::resizeGoban()
{ clear();                         // Rebuild the grids
}

/**
 * @brief Chiiki::clear - erase existing grids and lists
 *   in preparation for a new full search
 */
void Chiiki::clear()
{ foreach ( QVector<QPointer<Ryoiki> > grid, rGrid )
    grid.clear();
  rGrid.clear();
  foreach ( QList<QPointer<Ryoiki> > rpl, rpm )
    { foreach ( Ryoiki *rp, rpl )
        rp->deleteLater();
      rpl.clear();
    }
  rpm.clear();

  // Rebuild the empty grids
  if ( bp == nullptr )     { qDebug( "WARNING: Chiiki::clear() Goban null" ); return; }
  if ( tp == nullptr )     { qDebug( "WARNING: Chiiki::clear() Shiko null" ); return; }
  if ( tp->gp == nullptr ) { qDebug( "WARNING: Chiiki::clear() Game null" ); return; }
  qint32 nPoints = bp->nPoints();  // Number of points on the Goban
  qint32 np = tp->gp->np;          // Number of Players in this Game
  rGrid.reserve( np + 1 );
  QVector<QPointer<Ryoiki> > rgBlank;
  while ( rGrid.size() <= np )
    rGrid.append( rgBlank );
  for ( qint32 pl = 0 ; pl <= np ; pl++ )
    { rGrid[pl].reserve( nPoints );
      while ( rGrid.at(pl).size() < nPoints )
        rGrid[pl].append( nullptr );
    }

  // Rebuild the empty lists
  QList<QPointer<Ryoiki> > rlBlank;
  while ( rpm.size() <= np )
    rpm.append( rlBlank );
}

/**
 * @brief Chiiki::update - work through the Goban to set
 *   Hata everywhere there are no Goishi
 */
void Chiiki::update()
{ clear();
  if ( bp == nullptr )     { qDebug( "WARNING: Chiiki::update() Goban null" ); return; }
  if ( tp == nullptr )     { qDebug( "WARNING: Chiiki::update() Shiko null" ); return; }
  if ( tp->gp == nullptr ) { qDebug( "WARNING: Chiiki::update() Game null"  ); return; }
  qint32 np = tp->gp->np;
  Ryoiki *rp = nullptr;
  // Determine how many Ryoiki there are, and what coordinates they each cover
  // TODO: determine if there is any value in the per-player Ryoiki
  for ( qint32 pl = 0; pl <= np; pl++ )  // Once for each player, then one more time for all players
    { bool rule = false; // Searching for not c
      qint32 c = pl;
      if ( pl >= np )
        { c = NO_PLAYER; // Search for empty space
          rule = true;
        }
      if ( rp != nullptr )
        rp->player = pl;
      for ( qint32 x = 0; x < bp->Xsize; x++ )
        for ( qint32 y = 0; y < bp->Ysize; y++ )
          { qint32 i = bp->xyToIndex( x, y );
            if ( rGrid[pl].at(i) == nullptr )
              { if ( rp == nullptr )            // Need a new Ryoiki?
                  rp = new Ryoiki( pl, this );
                if ( rGrid[pl][i] == nullptr )         // Is there already a Ryoiki defined here?
                  if ( bp->fill( x, y, c, rule, rp ) ) // Did a new Ryoiki get placed?
                    { rpm[pl].append( rp );            // Add another Ryoiki to the matrix
                      foreach ( qint32 ii, rp->bi )
                        rGrid[pl][ii] = rp;            // Mark all grid points with the rp Ryoiki
                      rp = nullptr;
                    }
              }
          }
    }
  if ( rp != nullptr ) // Was the last Ryoiki unused?
    rp->deleteLater();
  // Determine the owner of each Ryoiki in the np grid
  for ( qint32 k = 0; k < rpm.at(np).size(); k++ )
    { rp = rpm.at(np).at(k);
      if ( rp == nullptr )
        qDebug( "Chiiki::update() Ryoiki null" );
       else
        { qint32 ro = UNDETERMINED_PLAYER;
          bool done = ( rp->bi.size() <= 0 );
          if ( done ) qDebug( "why are we here with no Goban indices in the Ryoiki? %d", k );
          qint32 j = 0;
          while( !done )
            { qint32 i = rp->bi.at(j);
              qint32 x,y;
              bp->indexToXY( i, &x, &y );
              if ( x > 0 )               done |= ryoikiOwner( x-1, y, &ro );
              if ( x < (bp->Xsize - 1) ) done |= ryoikiOwner( x+1, y, &ro );
              if ( y > 0 )               done |= ryoikiOwner( x, y-1, &ro );
              if ( y < (bp->Ysize - 1) ) done |= ryoikiOwner( x, y+1, &ro );
              if ( ++j >= rp->bi.size() ) done = true;
            }
          rp->owner = ro;
        }
    }

  // Determine all adjacent, relevant Wyrms to each Ryoiki
  for ( qint32 pl = 0; pl <= np; pl++ )
    for ( qint32 k = 0; k < rpm.at(pl).size(); k++ )
      { rp = rpm.at(pl).at(k);
        foreach ( qint32 i, rp->bi )
          { qint32 x,y;
            bp->indexToXY( i, &x, &y );
            if ( x > 0 )              collectWyrms( x-1, y, rp );
            if ( x < (bp->Xsize -1) ) collectWyrms( x+1, y, rp );
            if ( y > 0 )              collectWyrms( x, y-1, rp );
            if ( y < (bp->Ysize -1) ) collectWyrms( x, y+1, rp );
          }
      }
}

/**
 * @brief Chiiki::collectWyrms
 * @param x - coordinate to check
 * @param y - coordinate to check
 * @param rp - Ryoiki to collect Wyrms for
 */
void Chiiki::collectWyrms( qint32 x, qint32 y, Ryoiki *rp )
{ qint32 i   = bp->xyToIndex(x,y);
  Goishi *ip = bp->goishi(i);
  if ( ip == nullptr ) // Any Wyrm here to collect?
    return;
  qint32 pl = rp->player;
  if ( pl < tp->gp->np )   // Collect all colors?
    if ( pl != ip->color ) // Wyrm of a collectable color?
      return;
  Wyrm *wp = ip->wp;
  if ( wp == nullptr )
    { qDebug( "UNEXPECTED: Chiiki::collectWyrms() found a null Wyrm" );
      return;
    }
  if ( !rp->wpl.contains( wp ) )
    rp->wpl.append( wp );
}

/**
 * @brief Chiiki::ryoikiOwner - judging adjacent Goishi to a Ryoiki and the current color determination
 *   determine the continuing color evaluation of the Ryoiki
 * @param x - coordinate to evaluate
 * @param y - coordinate to evaluate
 * @param ro - current owner assignment
 * @return true if the current color assignment has become disputed
 */
bool Chiiki::ryoikiOwner( qint32 x, qint32 y, qint32 *ro )
{ if ( *ro == NO_PLAYER )
    return true; // Done before we start
  qint32 i = bp->xyToIndex(x,y);
  if (( i < 0 ) || ( i >= bp->nPoints() ))
    return true;
  Goishi *ip = bp->goishi( i );
  if ( ip == nullptr )
    return false; // No info here
  if ( *ro == UNDETERMINED_PLAYER )
    { *ro = ip->color;
      return false;
    }
  if ( *ro == ip->color )
    return false;   // Continuing the trend
  *ro = NO_PLAYER;  // Contrasting neighbor found, territory is in dispute
  return true;
}

/**
 * @brief Chiiki::colorAt
 * @param i - index of grid position to read
 * @return color (owner) of Ryoiki at i
 */
qint32  Chiiki::colorAt( qint32 i )
{ if ( bp == nullptr )                     { qDebug( "WARNING: Chiiki::colorAt() Goban null" ); return NO_PLAYER; }
  if ( tp == nullptr )                     { qDebug( "WARNING: Chiiki::colorAt() Shiko null" ); return NO_PLAYER; }
  if ( tp->gp == nullptr )                 { qDebug( "WARNING: Chiiki::colorAt() Game null"  ); return NO_PLAYER; }
  if (( i < 0 ) || ( i >= bp->nPoints() )) { qDebug( "Chiiki::colorAt(%d) out of range", i   ); return NO_PLAYER; }
  qint32 np = tp->gp->np;
  if ( rGrid.size() < np )                 { qDebug( "Chiiki::colorAt() rGrid too small %d %d", rGrid.size(), np );     return NO_PLAYER; }
  Ryoiki *rp = rGrid.at(np).at(i);
  if ( rp == nullptr )                     { qDebug( "Chiiki::colorAt(%d) not in a Ryoiki, unusual to request it", i ); return NO_PLAYER; }
  return rp->owner;
}

