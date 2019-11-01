#include "draco.h"

/**
 * @brief Draco::Draco - constructor
 * @param wp - First Wyrm in the Draco
 * @param awpl - list of Wyrms to consider adding
 * @param p
 */
Draco::Draco( Wyrm *wp, QList<Wyrm *> awpl, Shiko *p ) : QObject(p), bp(p->bp), tp(p)
{ wp->dp = this;
  wpl.append( wp );
  addConnections( awpl );
}

/**
 * @brief Draco::addConnections - build this Draco up from the
 *   Wyrms in pwpl, most solid connections first
 * @param pwpl - list of Wyrms to try to connect
 */
void Draco::addConnections( QList<Wyrm *> pwpl )
{ QList<Wyrm *> awpl; // Prune out Wyrms already in a(ny) Draco, and check for anomalies
  foreach ( Wyrm *wp, pwpl )
    if ( wp != nullptr )
      { if ( wp->color() == wpl.at(0)->color() )
          { if ( wp->dp == nullptr )
              awpl.append( wp );
          }
         else
          qDebug( "UNEXPECTED: Draco::addConnections() color mismatch" );
      }
    else
     qDebug( "UNEXPECTED: Draco::addConnections() Wyrm null" );
  if ( addSolidConnections( awpl ) ) addConnections( awpl );
  if ( addSingleConnection( awpl ) ) addConnections( awpl );
  if ( addDoubleConnection( awpl ) ) addConnections( awpl );
}

/**
 * @brief Draco::addSolidConnections - if any Wyrms in awpl are connected
 *   to this Draco by two or more single point cuts, add them to the Draco's
 *   wpl, point them to this Draco, and return true.
 * @param awpl - list of Wyrms to consider
 * @return true if any solid connections are found
 */
bool Draco::addSolidConnections( QList<Wyrm *> awpl )
{ bool added = false;
  foreach ( Wyrm *wp, wpl )
    foreach ( Wyrm *awp, awpl )
      if ( awp->dp == nullptr )
        if ( singleCutCount( wp, awp ) > 1 )
          { awp->dp = this; // Welcome to the Draco
            wpl.append( awp );
            added = true;
          }
  return added;
}

/**
 * @brief Draco::addSingleConnection - if any Wyrms in awpl are connected
 *   to this Draco by only one single point cut, add them to the Draco's
 *   wpl, point them to this Draco, add their cut point to the cut point list,
 *   and return true.
 * @param awpl - list of Wyrms to consider
 * @return true if a single connections is found (and made)
 */
bool Draco::addSingleConnection( QList<Wyrm *> awpl )
{ foreach ( Wyrm *wp, wpl )
    foreach ( Wyrm *awp, awpl )
      if ( awp->dp == nullptr )
        if ( singleCutCount( wp, awp ) == 1 )
          { awp->dp = this; // Welcome to the Draco
            wpl.append( awp );
            foreach ( qint32 i, wp->libertyList )
              if ( awp->libertyList.contains(i) )
                cutPoints.append( i );
            return true;
          }
  return false;
}

/**
 * @brief Draco::addDoubleConnection - if any Wyrm in awpl is connectable
 *   to this Draco by placement of two Goishi, add it to the Draco and
 *   add the two grid points to the cutPairs list.
 * @param awpl - list of Wyrms to consider
 * @return true if a double connection is found (and made)
 */
bool Draco::addDoubleConnection( QList<Wyrm *> awpl )
{ foreach ( Wyrm *wp, wpl )
    foreach ( Wyrm *awp, awpl )
      if ( awp->dp == nullptr )
        { QList<QPair<qint32,qint32> > cpl = doubleCutPairs( wp, awp );
          if ( cpl.size() > 0 )
            { awp->dp = this; // Welcome to the Draco
              wpl.append( awp );
              cutPairs.append( cpl );
              return true;
        }   }
  return false;
}

/**
 * @brief Draco::singleCutCount
 * @param wp - Wyrm to compare
 * @param awp - Wyrm to compare
 * @return number of liberties wp and awp have in common
 */
qint32 Draco::singleCutCount( Wyrm *wp, Wyrm *awp )
{ if (( wp->libertyList.size() < 2 ) && ( awp->libertyList.size() < 2 ))
    return 0; // If both are in Atari, joining them will do no good (if they are even joinable...)
  qint32 count = 0;
  foreach ( qint32 i, wp->libertyList )
    if ( awp->libertyList.contains(i) )
      count++;
  return count;
}

/**
 * @brief Draco::doubleCutPairs
 * @param wp - Wyrm to compare
 * @param awp - Wyrm to compare
 * @return list of point pairs which could connect the Wyrms
 */
QList<QPair<qint32,qint32> > Draco::doubleCutPairs( Wyrm *wp, Wyrm *awp )
{ QList<QPair<qint32,qint32> > cpl;
  foreach ( qint32 i, wp->libertyList )
    { if ( awp->libertyList.contains(i) )
        qDebug( "UNEXPECTED: Draco::doubleCutPairs found %d in both Wyrms", i );
      foreach ( qint32 j, awp->libertyList )
        if ( bp->indexNeighbors( i, j ) )
          { qint32 ci = wp->color();
            qint32 cj = awp->color();
            if ( ci != cj )
              qDebug( "UNEXPECTED: Draco::doubleCutPairs color mismatch %d %d", ci, cj );
             else
              { if ( ( tp->testLibertyCount( i, ci ) + tp->testLibertyCount( j, cj ) ) > 2 ) // Don't connect if both are Ataris (connection would be self-capture)
                  { QPair<qint32,qint32> pr;
                    if ( j > i )
                      pr = QPair<qint32,qint32>(i,j);
                     else
                      pr = QPair<qint32,qint32>(j,i);
                    cpl.append(pr);
                  }
              }
          }
    }
  return cpl;
}

/**
 * @brief Draco::color
 * @return color of this Draco
 */
qint32 Draco::color()
{ if ( wpl.size() < 1 )
    { qDebug( "UNEXPECTED: Draco::color no Wyrms" );
      return NO_PLAYER;
    }
  if ( wpl.at(0) == nullptr )
    { qDebug( "UNEXPECTED: Draco::color Wyrm null" );
      return NO_PLAYER;
    }
  return wpl.at(0)->color();
}

/**
 * @brief Draco::show - for debugging
 * @return string showing the contents of this Draco
 */
QString Draco::show()
{ QString msg = "Wyrms in Draco:\n";
  foreach ( Wyrm *wp, wpl )
    msg.append( wp->show() );
  msg.append( "Cut points: " );
  foreach ( qint32 i, cutPoints )
    msg.append( bp->indexToVertex(i)+" " );
  msg.append( "pairs: " );
  QPair<qint32,qint32> pair;
  foreach ( pair, cutPairs )
    msg.append( bp->indexToVertex(pair.first)+"-"+bp->indexToVertex(pair.second)+" " );
  msg.append( "\n" );
  return msg;
}
