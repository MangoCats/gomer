#include "chiiki.h"

/**
 * @brief Hata::Hata - basic constructor
 * @param p - Chiiki parent
 * @param pi - Goban index this Hata pertains to
 * @param pri - Ryoiki index this Hata pertains to
 */
Hata::Hata( Chiiki *p, qint32 pi, qint32 pri ) : QObject(p), i(pi), ri(pri), bp(p->bp)
{}

/**
 * @brief Hata::Hata - copy constructor
 * @param php - previous Hata to copy
 * @param p - new Chiiki parent
 */
Hata::Hata( Hata *php, Chiiki *p ) : QObject(p), bp(p->bp)
{ i       = php->i;
  ri      = php->ri;
}

qint32 Hata::x()
{ if ( bp == nullptr )
    return -1;
  qint32 x,y;
  bp->indexToXY(i,&x,&y);
  return x;
}

qint32 Hata::y()
{ if ( bp == nullptr )
    return -1;
  qint32 x,y;
  bp->indexToXY(i,&x,&y);
  return y;
}

QString Hata::show()
{ return bp->indexToVertex(i) + " "; }

/**
 * @brief Ryoiki::Ryoiki - a group of Hata, forms a region
 *   like a Wyrm of free space / liberties.
 * @param p - Chiiki parent
 */
Ryoiki::Ryoiki( Chiiki *p ) : QObject(p), bp(p->bp)
{ color = NO_PLAYER; // Unknown, at first
}

QString Ryoiki::show()
{ QString s = QString( "%1 " ).arg( bp->colorToChar( color ) );
  foreach ( Hata *hp, hpl )
    s.append( hp->show() );
  return s + "\n";
}

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
Chiiki::Chiiki(Chiiki *pcp,Shiko *p) : QObject(p), bp(pcp->bp), tp(p)
{ if ( bp == nullptr )
    { qDebug( "WARNING: Chiiki::copy constructor Goban null" );
      return;
    }
  resizeGoban();
  qint32 nPoints = bp->nPoints();
  qint32 nRyoiki = pcp->rpl.size();
  while ( rpl.size() < nRyoiki )
    rpl.append( new Ryoiki(this) );
  hGrid.reserve( nPoints );
  for ( qint32 i = 0 ; i < nPoints ; i++ )
    { Hata *hp = pcp->hGrid.at(i);
      if ( hp == nullptr )
        hGrid.append( nullptr );
       else
        { Hata *nhp = new Hata( hp, this );
          hGrid.append( nhp );
          ryoikiListAdd( nhp );
        }
    }
}

QString Chiiki::showRyoiki()
{ QString s;
  foreach ( Ryoiki *rp, rpl )
    s.append( rp->show() );
  return "\n" + s;
}

/**
 * @brief Chiiki::ryoikiListAdd
 * @param hp - Hata to add (ri preset)
 * @return true if successful
 */
bool Chiiki::ryoikiListAdd( Hata *hp )
{ if ( hp->ri >= rpl.size() )
    { qDebug( "UNEXPECTED: ri %d rpl.size() %d", hp->ri, rpl.size() );
      return false;
    }
  rpl.at(hp->ri)->addHata(hp);
  return true;
}

/**
 * @brief Chiiki::addHata - allocate a new Hata and store it
 * @param i - Goban index to place Hata at
 * @param ri - Ryoiki index to store Hata in
 * @return true if successful
 */
bool Chiiki::addHata( qint32 i, qint32 ri )
{ if (( i < 0 ) || ( i >= hGrid.size() ))
    { qDebug( "PROBLEM: Chiiki::addHata i %d hGrid.size() %d",i, hGrid.size() );
      return false;
    }
  if ( hGrid.at(i) != nullptr )
    { qDebug( "Chiiki::addHata expected nullptr at %d",i );
      return false;
    }
  Hata *hp = new Hata( this, i, ri );
  if ( !ryoikiListAdd( hp ) )
    { hp->deleteLater();
      return false;
    }
  hGrid.replace( i, hp );
  return true;
}

/**
 * @brief Chiiki::resizeGoban - also used for initial allocation
 *   make sure the hGrid has at least as many entries as the
 *   current Goban has grid points.
 */
void Chiiki::resizeGoban()
{ if ( bp == nullptr )
    { qDebug( "WARNING: Chiiki::resizeGoban() Goban null" );
      return;
    }
  qint32 nPoints = bp->nPoints();
  hGrid.reserve( nPoints );
  clear();
  while ( hGrid.size() < nPoints )
    hGrid.append( nullptr );
}

/**
 * @brief Chiiki::clear - erase existing objects
 *   in preparation for a new full search
 */
void Chiiki::clear()
{ for ( qint32 i = 0; i < hGrid.size() ; i++ )
    if ( hGrid.at(i) != nullptr )
      { hGrid.at(i)->deleteLater();
        hGrid.replace(i,nullptr);
      }
  foreach( Ryoiki *rp, rpl )
    rp->deleteLater();
  rpl.clear();
}

/**
 * @brief Chiiki::hata - bounds checked accessor of the hGrid
 * @param i - index to retrieve
 * @return Hata at i
 */
Hata *Chiiki::hata( qint32 i )
{ if (( i < 0 ) || ( i >= hGrid.size() ))
    { qDebug( "WARNING: Chiiki::hata(%d) out of range",i );
      return nullptr;
    }
  return hGrid.at(i);
}

/**
 * @brief Chiiki::hFill - floodfill unclaimed territory with
 *  Hata in Ryoiki index ri
 * @param x - coordinate to look for unfilled neighbors from
 * @param y - coordinate to look for unfilled neighbors from
 * @param ri - Ryoiki index to fill this territory with
 */
void Chiiki::hFill( int x, int y, int ri )
{ int i = bp->xyToIndex(x,y);
  if (( i < 0 ) || ( i >= bp->nPoints() ))
    return;
  if ( bp->goishi( i ) != nullptr )
    return; // floodfill search does not pass a stone
  addHata( i, ri );
  if ( x > 0 )                 hCheck( x-1, y, ri );
  if ( x < ( bp->Xsize - 1 ) ) hCheck( x+1, y, ri );
  if ( y > 0 )                 hCheck( x, y-1, ri );
  if ( y < ( bp->Ysize - 1 ) ) hCheck( x, y+1, ri );
}

/**
 * @brief Chiiki::hCheck
 * @param x - coordinate to check for stone
 * @param y - coordinate to check for stone
 * @param ri - Ryoiki index for these Hata
 */
void Chiiki::hCheck( int x, int y, int ri )
{ if ( bp == nullptr )
    return;
  int i = bp->xyToIndex( x,y );
  if (( i < 0 ) || ( i >= bp->nPoints() ))
    return; // Board is not ready yet, happens at first initialization
  if ( hGrid.size() < i )
    { qDebug( "ERROR: hGrid not ready yet" );
      return;
    }
  if ( hGrid.at( i ) != nullptr )
    { Hata *hp = hGrid.at(i);
      if ( hp->ri != ri )
        { QString msg = QString( "ERROR: hCheck @(%1,%2,%3) encountered another territory index %4" ).arg(x).arg(y).arg(ri).arg(hp->ri);
          qDebug( qPrintable( msg ) );
        }
      return; // already marked
    }
  if ( bp->color(i) > NO_PLAYER )
    return; // floodfill search does not pass a stone
  // Empty grid, continue the floodfill from here
  hFill( x, y, ri );
}


/**
 * @brief Chiiki::update - work through the Goban to set
 *   Hata everywhere there are no Goishi
 */
void Chiiki::update()
{ clear();
  // Determine how many Ryoiki there are, and what coordinates they each cover
  for ( int x = 0; x < bp->Xsize; x++ )
    for ( int y = 0; y < bp->Ysize; y++ )
      { qint32 i = bp->xyToIndex( x, y );
        if (( hata(i) == nullptr ) && ( bp->goishi(i) == nullptr ))
          { rpl.append( new Ryoiki(this) );
            hFill( x, y, rpl.size()-1 );
          }
      }
  // Determine the color of each Ryoiki
  for ( int k = 0; k < rpl.size(); k++ )
    { Ryoiki *rp = rpl.at(k);
      int rc = UNDETERMINED_PLAYER;
      bool done = ( rp->hpl.size() <= 0 );
      int j = 0;
      while( !done )
        { Hata *hp = rp->hpl.at(j);
          if ( hp->x() > 0 )               done |= ryoikiColor( hp->x()-1, hp->y(), &rc );
          if ( hp->x() < (bp->Xsize - 1) ) done |= ryoikiColor( hp->x()+1, hp->y(), &rc );
          if ( hp->y() > 0 )               done |= ryoikiColor( hp->x(), hp->y()-1, &rc );
          if ( hp->y() < (bp->Ysize - 1) ) done |= ryoikiColor( hp->x(), hp->y()+1, &rc );
          if ( ++j >= rp->hpl.size() ) done = true;
        }
      foreach ( Hata *hp, rp->hpl )
        rp->color = rc;
    }
}

/**
 * @brief Chiiki::ryoikiColor - judging adjacent Goishi to a Ryoiki and the current color determination
 *   determine the continuing color evaluation of the Ryoiki
 * @param x - coordinate to evaluate
 * @param y - coordinate to evaluate
 * @param rc - current color assignment
 * @return true if the current color assignment has become disputed
 */
bool Chiiki::ryoikiColor( int x, int y, int *rc )
{ int i = bp->xyToIndex(x,y);
  if ( bp->nPoints() <= i )
    return true;
  Goishi *ip = bp->goishi( i );
  if ( ip == nullptr )
    return false; // No info here
  if ( *rc == NO_PLAYER )
    return true; // Done before we start
  if ( *rc == UNDETERMINED_PLAYER )
    { *rc = ip->color;
      return false;
    }
  if ( *rc == ip->color )
    return false; // Continuing the trend
  *rc = NO_PLAYER;       // Contrasting neighbor found, territory is in dispute
  return true;
}

