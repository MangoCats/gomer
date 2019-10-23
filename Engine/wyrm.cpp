#include "wyrm.h"

/**
 * @brief Wyrm::Wyrm
 * @param ip - single Goishi Wyrm
 * @param p - parent
 */
Wyrm::Wyrm(Goishi *ip, Shiko *p) : QObject(p), tp(p)
{ addGoishi( ip ); }

/**
 * @brief Wyrm::Wyrm - copy constructor
 * @param wp - Wyrm to copy
 * @param parent - parent of the new Wyrm
 */
Wyrm::Wyrm(Wyrm *wp, Shiko *p) : QObject(p), tp(p)
{ Goban *bpn = p->bp; // New Goban
  foreach( Goishi *ipo, wp->ipl )
    { Goishi *ipn = bpn->goishiAt( ipo->x, ipo->y );
      if ( ipn == nullptr )
        qDebug( "ERROR: nullptr in copied Wyrm" );
       else
        { ipn->wp = this;
          ipl.append( ipn );
        }
    }
  libertyList = wp->libertyList;
}

/**
 * @brief Wyrm::addGoishi - add one Goishi to this Wyrm
 * @param ip
 */
void Wyrm::addGoishi( Goishi *ip )
{ if ( ip == nullptr )
    { qDebug( "Wyrm::addGoishi unexpected Goishi nullptr" );
      return;
    }
  if ( ipl.size() > 0 )
    { if ( ipl.at(0) == nullptr )
        { qDebug( "Wyrm::addGoishi unexpected Goishi(0) nullptr" );
          return;
        }
      if ( ipl.at(0)->color != ip->color )
        { qDebug( "Wyrm::addGoishi color mismatch %d %d", ipl.at(0)->color, ip->color );
          return;
        }
      Goban *bp = ip->bp;
      if ( bp == nullptr )
        { qDebug( "Wyrm::addGoishi WARNING: Goishi Goban nullptr" );
          return;
        }
      mergeLibertyList( getLibertyList( ip ) );
      qint32 i = bp->xyToIndex( ip->x, ip->y );
      if ( libertyList.contains( i ) )
        libertyList.removeAt( libertyList.indexOf( i ) );
       // can happen that the liberty list doesn't contain i when we are merging 2 or more Wyrms with a newly placed Goishi
    }
   else
    libertyList = getLibertyList( ip );

  ipl.append( ip );
  ip->wp = this;
}

/**
 * @brief Wyrm::mergeLibertyList, add any unique liberties not in the current liberty list
 * @param ll - liberty list to merge
 */
void  Wyrm::mergeLibertyList( QList<qint32> ll )
{ foreach ( qint32 l, ll )
    if ( !libertyList.contains(l) )
      libertyList.append(l);
  std::sort(libertyList.begin(), libertyList.end()); // qSort(libertyList);
}

/**
 * @brief Wyrm::getLibertyList
 * @param ip - Goishi pointer
 * @return list of all liberties this Goishi would have if placed at ip->x, ip->y
 */
QList<qint32> Wyrm::getLibertyList( Goishi *ip )
{ QList<qint32>ll;
  if ( ip == nullptr )
    { qDebug( "Wyrm::getLibertyList unexpected Goishi nullptr" );
      return ll;
    }
  Goban *bp = ip->bp;
  if ( bp == nullptr )
    { qDebug( "Wyrm::getLibertyList unexpected Goishi-Goban nullptr" );
      return ll;
    }
  qint32 x = ip->x;
  qint32 y = ip->y;
  if ( x > 0 )
    if ( bp->goishiAt( x-1, y ) == nullptr )
      ll.append( bp->xyToIndex(x-1, y) );
  if ( x < bp->Xsize - 1 )
    if ( bp->goishiAt( x+1, y ) == nullptr )
      ll.append( bp->xyToIndex(x+1, y) );
  if ( y > 0 )
    if ( bp->goishiAt( x, y-1 ) == nullptr )
      ll.append( bp->xyToIndex(x, y-1) );
  if ( y < bp->Ysize - 1 )
    if ( bp->goishiAt( x, y+1 ) == nullptr )
      ll.append( bp->xyToIndex(x, y+1) );
  return ll;
}

/**
 * @brief Wyrm::addWyrm - add the passed Wyrm's Goishi to this Wyrm
 * @param wp - Wyrm to add
 */
void Wyrm::addWyrm( Wyrm *wp )
{ foreach ( Goishi *ip, wp->ipl )
    addGoishi( ip );
}

/**
 * @brief Wyrm::color
 * @return color of this Wyrm
 */
qint32 Wyrm::color()
{ if ( ipl.size() <= 0 )
    return -1;
  return ipl.at(0)->color;
}

/**
 * @brief Wyrm::removeLiberty - expected to reduce liberty count by 1, but not to zero
 * @param i - index of liberty position to remove
 * @return true if the liberty was removed, false if it was not in the list
 */
bool Wyrm::removeLiberty( qint32 i )
{ if ( !libertyList.contains(i) )
    return false;
  libertyList.removeAt( libertyList.indexOf( i ) );
  if ( libertyList.size() == 0 )
    qDebug( "Wyrm::removeLiberty unexpected capture! %s", qPrintable( show() ) );
  return true;
}

/**
 * @brief Wyrm::addLiberty
 * @param i - index of liberty to add
 * @return true if added
 */
bool Wyrm::addLiberty( qint32 i )
{ if ( libertyList.contains(i) )
    return false;
  libertyList.append(i);
  std::sort(libertyList.begin(), libertyList.end()); // qSort(libertyList);
  return true;
}

/**
 * @brief Wyrm::show
 * @return debug description of the Wyrm and its liberties
 */
QString Wyrm::show()
{ if ( ipl.size() <= 0 )
    return "Empty Wyrm, no Goishi\n";
  QString w = QString::number( color() )+" ";
  Goban *bp = ipl.at(0)->bp;
  if ( bp == nullptr )
    return "Goban nullptr\n";
  foreach( Goishi *ip, ipl )
    w.append( bp->xyToVertex( ip->x, ip->y )+" " );
  w.append( "Libs:" );
  foreach( qint32 i, libertyList )
    w.append( " " + bp->indexToVertex( i ) );
  w.append( "\n" );
  return w;
}

/**
 * @brief Wyrm::adjacentRyoiki
 * @return a list of all Ryoiki this Wyrm touches
 */
QList<Ryoiki *> Wyrm::adjacentRyoiki()
{ QList<Ryoiki *> arpl;
  if ( tp == nullptr )
    { qDebug( "Wyrm::adjacentRyoiki() Shiko null" );
      return arpl;
    }
  Chiiki *cp = tp->cp;
  if ( cp == nullptr )
    { qDebug( "Wyrm::adjacentRyoiki() Chiiki null" );
      return arpl;
    }
  Goban *bp = cp->bp;
  if ( bp == nullptr )
    { qDebug( "Wyrm::adjacentRyoiki() Goban null" );
      return arpl;
    }
  foreach ( Goishi *ip, ipl ) // Work through each Goishi in this Wyrm
    { if ( ip == nullptr )
        qDebug( "Wyrm::adjacentRyoiki() Goishi null" );
       else
        { qint32 x = ip->x;
          qint32 y = ip->y;
          if ( x > 0 )            addRyoiki( cp, x-1, y, arpl );
          if ( x < bp->Xsize -1 ) addRyoiki( cp, x+1, y, arpl );
          if ( y > 0 )            addRyoiki( cp, x, y-1, arpl );
          if ( y < bp->Ysize -1 ) addRyoiki( cp, x, y+1, arpl );
        }
    }
  return arpl;
}

/**
 * @brief Wyrm::addRyoiki - read a single grid point looking for
 *   novel Ryoiki to add to the list.
 * @param cp - Chiiki to get the Ryoiki from
 * @param x - grid coordinate to check
 * @param y - grid coordinate to check
 * @param arpl - adjacent Ryoiki pointer list
 */
void Wyrm::addRyoiki( Chiiki *cp, qint32 x, qint32 y, QList<Ryoiki *>& arpl )
{ qint32 i = cp->bp->xyToIndex(x,y);
  qint32 np = cp->tp->gp->np;
  if (( i < 0 ) || ( i >= cp->bp->nPoints() ))
    { qDebug( "Wyrm::addRyoiki() %d, %d off Goban",x,y );
      return;
    }
  if ( cp->rGrid.at(np).at(i) == nullptr )
    return;
  Ryoiki *rp = cp->rGrid.at(np).at(i);
  if ( rp == nullptr )
    { qDebug( "Wyrm::addRyoiki() Ryoiki null" );
      return;
    }
  if (( rp->owner != color() ) && ( rp->owner != NO_PLAYER ))
    { qDebug( "Wyrm::addRyoiki() Ryoiki color %d mismatch to Wyrm color %d", rp->owner, color() );
      return;
    }
  if ( !arpl.contains( rp ) )
    arpl.append( rp );
}
