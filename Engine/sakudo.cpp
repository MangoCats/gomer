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

      default:
      case 2: return genmoveKilroy(c);

      case 3: return genmoveTerry(c);
    }
}

/**
 * @brief Sakudo::genmoveRandy - return a random legal move, pass if none available
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveRandy( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = tp->allLegalMoves(c);
  if ( lml.size() < 1 )
    return "pass";
  return bp->indexToVertex(lml.at( rng.bounded(0,lml.size()) ));
}

/**
 * @brief Sakudo::genmoveKilroy - return a move that captures the most opponents stones possible
 *   Kilroy is relentlessly aggressive, and not smart at all.  Lack of any defense or attention
 *   to building life means Kilroy will not be winning many games.
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveKilroy( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = tp->allLegalMoves(c);
  if ( lml.size() < 1 )
    return "pass";
  qint32 x,y;
  // First move?
  if ( tp->stateHistory.size() <= 1 ) // TODO: tweak this up for large vs small boards
    { x = bp->Xsize - 4; if ( x < 0 ) x = 0;
      y = bp->Ysize - 4; if ( y < 0 ) y = 0;
      return bp->xyToVertex(x,y);
    }

  // Make a list of opponent Wyrms which can be attacked, and find the minimum liberties on any attackable Wyrm
  QList<Wyrm *>owpl;
  qint32 minLib = bp->nPoints();
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveKilroy null Wyrm"); }
       else
        { if ( canBeAttacked( wp, c ) )
            { owpl.append(wp);
              if ( wp->libertyList.size() < minLib )
                minLib = wp->libertyList.size();
            }
        }
    }
  if ( owpl.size() < 1 )    // Nothing to attack, make a random move
    return genmoveRandy(c);

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
    { qDebug( "SURPRISED: Sakudo::genmoveKilroy awpl empty" );
      return genmoveRandy(c);
    }
  QList<qint32> aapl; // Available attack points list
  foreach ( Wyrm *wp, awpl )
    foreach ( qint32 i, wp->libertyList )
      if ( tp->legalMoveIndex(i,c) )
        aapl.append( i );

  if ( aapl.size() < 1 )
    { qDebug( "SURPRISED: Sakudo::genmoveKilroy aapl.size() == 0" );
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
    { qDebug( "SURPRISED: Sakudo::genmoveKilroy mapl.size() == 0" );
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
 * @return true if one of the Wyrm's liberties is also a legal move
 */
bool Sakudo::canBeAttacked( Wyrm *wp, qint32 c )
{ if ( wp->color() == c )
    return false;
  foreach ( qint32 i, wp->libertyList )
    if ( tp->legalMoveIndex(i,c) )
      // TODO: check if this Wyrm has unassailable life
      return true;
  return false;
}

/**
 * @brief Sakudo::genmoveTerry - return a move that adds as much territory as possible
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveTerry( qint32 c )
{ return "pass";

}
