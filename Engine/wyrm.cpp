#include "wyrm.h"

/**
 * @brief Wyrm::Wyrm
 * @param ip - single Goishi Wyrm
 * @param parent
 */
Wyrm::Wyrm(Goishi *ip, Shiko *parent) : QObject(parent)
{ addGoishi( ip ); }

/**
 * @brief Wyrm::Wyrm - copy constructor
 * @param wp - Wyrm to copy
 * @param parent - parent of the new Wyrm
 */
Wyrm::Wyrm(Wyrm *wp, Shiko *parent) : QObject(parent)
{ Goban *bpn = parent->bp; // New Goban
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
