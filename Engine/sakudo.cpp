#include "sakudo.h"

Sakudo::Sakudo(Game *p) : QObject(p), gp(p), bp(p->bp), tp(p->tp)
{ level = 0;  // Currently defaulting to Randy
  rng.seed( 1 );
}

/**
 * @brief Sakudo::genmove
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmove( qint32 c )
{ if ( gp == nullptr )
    { qDebug( "WARNING: Sakudo::genmove Game null" );
      return "pass";
    }
  if ( gp->pt != c )
    return "pass";  // Not c's turn

  switch ( level )
    { case 1: return genmoveRandy(c);

      case 2: return genmoveKilgore(c);

      default:
      case 3: return genmoveKilroy(c);

      case 4: return genmoveTerry(c);
    }
}


/**
 * @brief Sakudo::allInOwnRyoiki
 * @param c - color to check
 * @param ml - move list (indices)
 * @return true if every index in ml is in a Ryoiki owned by c
 */
bool Sakudo::allInOwnRyoiki( qint32 c, const QList<qint32>& ml )
{ Chiiki *cp = tp->cp;
  if ( cp == nullptr )
    { qDebug( "Sakudo::allInOwnRyoiki Chiiki null" );
      return false;
    }
  foreach ( qint32 i, ml )
    if ( cp->colorAt(i) != c )
      return false;
  return true;
}

QList<qint32> Sakudo::removeOwnRyoiki( qint32 c, const QList<qint32>& ml )
{ QList<qint32> sml;
  foreach ( qint32 i, ml )
    if ( tp->cp->colorAt(i) != c )
      sml.append(i);
  return sml;
}

/**
 * @brief Sakudo::genmoveRandy - return a random legal move, pass if none available
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveRandy( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = removeOwnRyoiki( c, tp->allLegalMoves(c) );
  if ( lml.size() < 1 )
    return "pass";
  return bp->indexToVertex(lml.at( rng.bounded(0,lml.size()) ));
}

/**
 * @brief Sakudo::genmoveKilgore - return a move that captures the most opponents stones possible
 *   Kilroy is relentlessly aggressive, and not smart at all.  Lack of any defense or attention
 *   to building life means Kilroy will not be winning many games.
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveKilgore( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = tp->allLegalMoves(c);
  if (( lml.size() < 1 ) || allInOwnRyoiki( c, lml ))
    return "pass";
  qint32 x,y;
  // First move?
  if ( tp->stateHistory.size() <= 1 )
    { qint32 dx = ( bp->Xsize < 13 ) ? 3 : 4;
      qint32 dy = ( bp->Ysize < 13 ) ? 3 : 4;
      x = bp->Xsize - dx; if ( x < 0 ) x = 0;
      y = bp->Ysize - dy; if ( y < 0 ) y = 0;
      if ( bp->color(bp->xyToIndex(x,y)) != NO_PLAYER )
        { x = dx - 1; if ( x >= bp->Xsize ) x = bp->Xsize - 1;
          y = dy - 1; if ( y >= bp->Ysize ) y = bp->Ysize - 1;
        }
      return bp->xyToVertex(x,y);
    }

  // Make a list of opponent Wyrms which can be attacked, and find the minimum liberties on any attackable Wyrm
  QList<Wyrm *>owpl;
  qint32 minLib = bp->nPoints();
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveKilgore null Wyrm"); }
       else
        { if ( canBeAttacked( wp, c ) )
            { owpl.append(wp);
              if ( wp->libertyList.size() < minLib )
                minLib = wp->libertyList.size();
            }
        }
    }
  if ( owpl.size() < 1 )    // Nothing to attack, make a random move on the first turn
    { if ( tp->stateHistory.size() <= gp->np )
        return genmoveRandy(c);
      return "pass";
    }

  // Of the attackable Wyrms with minLib liberties, which has the most Goishi?
  qint32 maxGoishi = 0;
  foreach ( Wyrm *wp, owpl )
    if ( wp->libertyList.size() == minLib )
      if ( wp->ipl.size() > maxGoishi )
        maxGoishi = wp->ipl.size();

  // Make a list of all attackable Wyrms with minLib liberties and maxGoishi
  QList<Wyrm *>awpl;
  foreach ( Wyrm *wp, owpl )
    if ( wp->libertyList.size() <= minLib )
      if ( wp->ipl.size() >= maxGoishi )
        awpl.append(wp);

  if ( awpl.size() < 1 )
    { qDebug( "SURPRISED: Sakudo::genmoveKilgore awpl empty" );
      return "pass";
    }

  QList<qint32> aapl; // Available attack points list
  foreach ( Wyrm *wp, awpl )
    foreach ( qint32 i, wp->libertyList )
      if ( tp->legalMoveIndex(i,c) )
        aapl.append( i );

  if ( aapl.size() < 1 )
    { qDebug( "SURPRISED: Sakudo::genmoveKilgore aapl.size() == 0" );
      return genmoveRandy(c);
    }

  // Just one attack point, nothing further to optimize
  if ( aapl.size() == 1 )
    return bp->indexToVertex(aapl.at( 0 ));

  // Look for attack points which impact the maximum number of opponent Wyrms
  qint32 maxImpact = 0;
  foreach( qint32 i, aapl )
    { qint32 impact = 0;
      foreach ( Wyrm *wp, owpl )
        if ( wp->libertyList.contains( i ) )
          impact++;
      if ( impact > maxImpact )
        maxImpact = impact;
    }
  QList<qint32> mapl;
  foreach( qint32 i, aapl )
    { qint32 impact = 0;
      foreach ( Wyrm *wp, owpl )
        if ( wp->libertyList.contains( i ) )
          impact++;
      if ( impact >= maxImpact )
        mapl.append(i);
    }

  if ( mapl.size() < 1 )
    { qDebug( "SURPRISED: Sakudo::genmoveKilgore mapl.size() == 0" );
      return genmoveRandy(c);
    }

  // Just one attack point, nothing further to optimize
  if ( mapl.size() == 1 )
    return bp->indexToVertex(mapl.at( 0 ));

  // Which point joins the maximum number of friendly Goishi
  QList<Wyrm *>fwpl;
  foreach ( Wyrm *wp, tp->wpl )
    foreach ( qint32 i, mapl )
      if ( wp->color() == c )
        if ( wp->libertyList.contains( i ) )
          fwpl.append( wp );

  qint32 mxGoishi = 0;
  foreach( qint32 i, mapl )
    { qint32 count = 0;
      foreach ( Wyrm *wp, fwpl )
        if ( wp->libertyList.contains( i ) )
          count += wp->ipl.size();
      if ( count > mxGoishi )
        mxGoishi = count;
    }
  QList<qint32> xapl;
  foreach( qint32 i, mapl )
    { qint32 count = 0;
      foreach ( Wyrm *wp, fwpl )
        if ( wp->libertyList.contains( i ) )
          count++;
      if ( count >= mxGoishi )
        xapl.append(i);
    }

  // No friendly Wyrms touching, choose at random among the previous list
  if ( xapl.size() < 1 )
    return bp->indexToVertex(mapl.at( rng.bounded(0,mapl.size()) ));

  // Just one attack point, nothing further to optimize
  if ( xapl.size() == 1 )
    return bp->indexToVertex(xapl.at( 0 ));

  // Choosing at random from equivalent options
  return bp->indexToVertex(xapl.at( rng.bounded(0,xapl.size()) ));
}

/**
 * @brief Sakudo::canBeAttacked
 * @param wp - Wyrm to check
 * @param c - color to try to attack
 * @return true if one of the Wyrm's liberties is also a legal move for c
 */
bool Sakudo::canBeAttacked( Wyrm *wp, qint32 c )
{ if ( wp->color() == c )
    return false;
  if ( wp->lifeOrDeath == WYRM_LIVE )
    return false;
  foreach ( qint32 i, wp->libertyList )
    if ( tp->legalMoveIndex(i,c,true) )
      return true;
  return false;
}

/**
 * @brief Sakudo::genmoveKilroy - a little less aggressive than Kilgore, Kilroy at least
 *   doesn't put himself in immediate Atari.
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveKilroy( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = tp->allLegalMoves(c);
  if (( lml.size() < 1 ) || allInOwnRyoiki( c, lml ))
    return "pass";
  qint32 x,y;
  // First move?
  if ( tp->stateHistory.size() <= 1 )
    { qint32 dx = ( bp->Xsize < 13 ) ? 3 : 4;
      qint32 dy = ( bp->Ysize < 13 ) ? 3 : 4;
      x = bp->Xsize - dx; if ( x < 0 ) x = 0;
      y = bp->Ysize - dy; if ( y < 0 ) y = 0;
      if ( bp->color(bp->xyToIndex(x,y)) != NO_PLAYER )
        { x = dx - 1; if ( x >= bp->Xsize ) x = bp->Xsize - 1;
          y = dy - 1; if ( y >= bp->Ysize ) y = bp->Ysize - 1;
        }
      return bp->xyToVertex(x,y);
    }

  // Make a list of opponent Wyrms which can be attacked, and find the minimum liberties on any attackable Wyrm
  QList<Wyrm *>owpl;
  qint32 minLib = bp->nPoints();
  qint32 maxSize = 0;
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveKilroy null Wyrm"); }
       else
        { if ( canBeAttacked( wp, c ) )
            { owpl.append(wp);
              if ( wp->libertyList.size() < minLib )
                minLib = wp->libertyList.size();
              if ( wp->libertyList.size() == 1 ) // While we're at it, find the largest Atari...
                if ( wp->ipl.size() > maxSize )
                  maxSize = wp->ipl.size();
            }
        }
    }

  if ( owpl.size() < 1 ) // Nothing to attack, cut it short for now
    return genmoveEasyD(c);

  if ( minLib <= 0 )
    { qDebug( "UNEXPECTED: Sakudo::genmoveKilroy %d minLib!", minLib );
      return genmoveEasyD(c);
    }

  if ( minLib == 1 )
    { // There is at least one Atari, which shall we take? The biggest one, of course.
      owpl.clear();
      foreach ( Wyrm *wp, tp->wpl )
        { if ( wp == nullptr )
            { qDebug( "WARNING: Sakudo::genmoveKilroy null Wyrm"); }
           else
            { if ( canBeAttacked( wp, c ) )
                if ( wp->libertyList.size() == 1 )
                  if ( wp->ipl.size() == maxSize )
                    owpl.append(wp);
            }
        }
      if ( owpl.size() < 1 )
        { qDebug( "UNEXPECTED: Sakudo::genmoveKilroy attackable Wyrm not refound" );
          return genmoveRandy(c);
        }
      // A brighter player might choose the "best" target to take, Kilroy is so lacking in brightness
      // I doubt there will often be a selection available
      return bp->indexToVertex(owpl.at(0)->libertyList.at(0));
    }

  QList<Wyrm *>fwpl;  // Friendly Wyrms to try to protect list
  qint32 oc = c+1; if ( oc >= tp->gp->np ) oc = 0; // An opponent color
  if ( minLib > 2 ) // Look for opportunities to relieve our own Ataris
    { foreach ( Wyrm *wp, tp->wpl )
        { if ( wp == nullptr )
            { qDebug( "WARNING: Sakudo::genmoveKilroy null Wyrm"); }
           else
            { if ( wp->color() == c )
                if ( canBeAttacked( wp, oc ) )
                  if ( wp->libertyList.size() == 1 )
                    if ( tp->testLibertyCount( wp->libertyList.at(0), c ) > 1 ) // TODO: evaluate ladders later...
                      fwpl.append(wp);
            }
        }
      if ( fwpl.size() == 1 )
        return bp->indexToVertex( fwpl.at(0)->libertyList.at(0) );
      if ( fwpl.size() > 1 )
        return bp->indexToVertex( fwpl.at( rng.bounded(0,fwpl.size()) )->libertyList.at(0) );
    }

  // owpl lists opponent Wyrms which can be attacked, minimum number of liberties is minLib, which is > 1
  // Let's see if any of those Wyrms with minLib liberties can be attacked without an immediate Atari on the attacking Goishi
  qint32 j          =  0;
  qint32 nextMinLib =  bp->nPoints();
  qint32 maxLc      =  0;
  qint32 maxLcI     = -1;
  while ( j < owpl.size() )
    { Wyrm *wp    = owpl.at(j);
      qint32 llsz = wp->libertyList.size();
      if ( llsz > minLib )
        { if ( llsz < nextMinLib )
            nextMinLib = llsz; // Be paying attention for the next round
          j++;
        }
       else // Working on a minLib Wyrm now, looking for a maxLc on this Wyrm's attackable points
        { foreach ( qint32 i, wp->libertyList )
            { if ( tp->legalMoveIndex( i, c ) )
                { qint32 lc = tp->testLibertyCount( i, c );
                  if ( lc > maxLc )
                    { maxLc  = lc;
                      maxLcI = i;
                    }
                }
            }
          owpl.removeAt( j ); // only self Ataris available when attacking this Wyrm's liberties, remove it from consideration
        } // else means owpl.at(j)->libertyList.size() == minLib

      if ( j >= owpl.size() )
        if ( maxLc < 2 ) // Failed to find a non-Atari in the Wyrms with minLib
          { if ( owpl.size() <= 0 ) // Out of attackable Wyrms, bail for now.
              return genmoveRandy(c);
            j = 0;               // Restart the loop
            minLib = nextMinLib; // Searching again, on the next minLib up
            nextMinLib = bp->nPoints();
          }
    } // while ( j < owpl.size() )

  if ( maxLc > 1 )
    return bp->indexToVertex( maxLcI );
  qDebug( "UNEXPECTED: only Ataris availble here" );
  return genmoveRandy(c);
}

/**
 * @brief Sakudo::genmoveEasyD - look for easy defense points
 * @param c - color to generate a move for
 * @return desired move
 */
QString Sakudo::genmoveEasyD( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = tp->allLegalMoves(c);
  if (( lml.size() < 1 ) || allInOwnRyoiki( c, lml ))
    return "pass";
  qint32 x,y;
  // First move?
  if ( tp->stateHistory.size() <= 1 )
    { qint32 dx = ( bp->Xsize < 13 ) ? 3 : 4;
      qint32 dy = ( bp->Ysize < 13 ) ? 3 : 4;
      x = bp->Xsize - dx; if ( x < 0 ) x = 0;
      y = bp->Ysize - dy; if ( y < 0 ) y = 0;
      if ( bp->color(bp->xyToIndex(x,y)) != NO_PLAYER )
        { x = dx - 1; if ( x >= bp->Xsize ) x = bp->Xsize - 1;
          y = dy - 1; if ( y >= bp->Ysize ) y = bp->Ysize - 1;
        }
      return bp->xyToVertex(x,y);
    }

  QList<Wyrm *>fwpl;  // Friendly Wyrms to try to protect list
  qint32 oc = c+1; if ( oc >= tp->gp->np ) oc = 0; // An opponent color
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveEasyD null Wyrm"); }
       else
        { if ( wp->color() == c )
            if ( canBeAttacked( wp, oc ) )
              if ( wp->libertyList.size() == 1 )
                if ( tp->testLibertyCount( wp->libertyList.at(0), c ) > 2 ) // TODO: evaluate ladders later...
                  fwpl.append(wp);
        }
    }
  if ( fwpl.size() == 1 )
    return bp->indexToVertex( fwpl.at(0)->libertyList.at(0) );
  if ( fwpl.size() > 1 )
    return bp->indexToVertex( fwpl.at( rng.bounded(0,fwpl.size()) )->libertyList.at(0) );

  // TODO: more defensive ideas
  return genmoveRandy(c);
}


/**
 * @brief Sakudo::genmoveTerry - return a move that adds as much territory as possible
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveTerry( qint32 c )
{ return "pass";

}
