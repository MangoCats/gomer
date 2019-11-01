#include "sakudo.h"
#include <QTimer>

/**
 * @brief Sakudo::Sakudo - constructor
 * @param p - Game, parent object
 */
Sakudo::Sakudo(Game *p) : QObject(p), gp(p), bp(p->bp), tp(p->tp)
{ level = 0;  // Currently defaulting to Monty
  rng.seed( 1 );
}

/**
 * @brief Sakudo::Sakudo - copy constructor
 * @param mp - Sakudo to copy
 * @param p - Parent of the new Sakudo
 */
Sakudo::Sakudo(Sakudo *mp, Game *p) : QObject(p), gp(p), bp(p->bp), tp(p->tp)
{ level = mp->level;  // Currently defaulting to Randy
  rng   = mp->rng;    // Copy the state of the parent's rng
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

      case 3: return genmoveKilkenny(c);

  default:
      case 4: return genmoveKilmer(c);

      case 5: return genmoveTerry(c);

      case 6: return genmoveMonty(c);
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
  return removePassEyes( sml );
}

QList<qint32> Sakudo::removePassEyes( const QList<qint32>& ml )
{ QList<qint32> sml = ml;
  foreach ( Wyrm *wp, tp->wpl )
    foreach ( qint32 i, wp->passEyes )
      if ( sml.contains(i) )
        sml.removeAll( i );
  return sml;
}

/**
 * @brief Sakudo::genmoveRandy - return a random legal move, pass if none available
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveRandy( qint32 c )
{ emit echo( QString( "# genmoveRandy(%1)" ).arg(c) );
  if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = removeOwnRyoiki( c, tp->allLegalMoves(c) );
  QList<qint32> naml;
  foreach ( qint32 i, lml )
    if ( tp->testLibertyCount(i,c) >= 2 )
      naml.append(i);
  if ( naml.size() < 1 )
    return "pass";
  return bp->indexToVertex(naml.at( rng.bounded(0,naml.size()) ));
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
  QList<qint32> lml = removeOwnRyoiki( c, tp->allLegalMoves(c) );
  if ( lml.size() < 1 )
    return "pass";

  if ( tp->stateHistory.size() <= 1 )
    return firstMove(c);

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
 * @brief Sakudo::firstMove
 * @param c - color to move for
 * @return a "stock" first move
 */
QString Sakudo::firstMove( qint32 c )
{ qint32 x,y;
  qint32 dx = ( bp->Xsize < 13 ) ? 3 : 4;
  qint32 dy = ( bp->Ysize < 13 ) ? 3 : 4;
  x = bp->Xsize - dx; if ( x < 0 ) x = 0;
  y = bp->Ysize - dy; if ( y < 0 ) y = 0;
  if ( bp->color(bp->xyToIndex(x,y)) != NO_PLAYER )
    { x = dx - 1; if ( x >= bp->Xsize ) x = bp->Xsize - 1;
      y = dy - 1; if ( y >= bp->Ysize ) y = bp->Ysize - 1;
    }
  if ( bp->color(bp->xyToIndex(x,y)) != NO_PLAYER )
    return genmoveRandy(c);
  return bp->xyToVertex(x,y);
}

/**
 * @brief Sakudo::genmoveKilkenny - a little less aggressive than Kilgore, Kilkenny at least
 *   doesn't put himself in immediate Atari.  Still, pretty dumb overall.
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveKilkenny( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = removeOwnRyoiki( c, tp->allLegalMoves(c) );
  if ( lml.size() < 1 )
    return "pass";
  if ( tp->stateHistory.size() <= 1 )
    return firstMove(c);

  // Make a list of opponent Wyrms which can be attacked, and find the minimum liberties on any attackable Wyrm
  QList<Wyrm *>owpl;
  qint32 minLib = bp->nPoints();
  qint32 maxSize = 0;
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveKilkenny null Wyrm"); }
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
    { qDebug( "UNEXPECTED: Sakudo::genmoveKilkenny %d minLib!", minLib );
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
        { qDebug( "UNEXPECTED: Sakudo::genmoveKilkenny attackable Wyrm not refound" );
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
            { qDebug( "WARNING: Sakudo::genmoveKilkenny null Wyrm"); }
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

QList<qint32> Sakudo::allPassEyes()
{ QList<qint32>passEyes;
  foreach ( Wyrm *wp, tp->wpl )
    foreach ( qint32 i, wp->passEyes )
      if ( !passEyes.contains(i) )
        passEyes.append(i);
  return passEyes;
}

/**
 * @brief Sakudo::genmoveKilmer - aka Iceman, trying to be a little less agressive
 *   in the name of self preservation
 * @param c - color to generate a move for
 * @return desired move
 */
QString Sakudo::genmoveKilmer( qint32 c )
{ emit echo( QString( "# genmoveKilmer(%1)" ).arg(c) );
  if (( tp == nullptr ) || ( bp == nullptr )) return "pass";
  QList<qint32> lml = removeOwnRyoiki( c, tp->allLegalMoves(c) );
  emit echo( QString( "# %1 legal moves available" ).arg(lml.size()) );
  if ( lml.size() < 1 )                       return "pass";
  if ( tp->stateHistory.size() <= 1 )         return firstMove(c);

  QList<qint32>passEyes = allPassEyes();

  // Make a list of opponent Wyrms which can be attacked
  QList<Wyrm *>owpl;
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveKilmer null Wyrm"); }
       else
        { if (( canBeAttacked( wp, c ) ) && ( aNotInB( wp->libertyList, passEyes ) ))
            owpl.append(wp);
        }
    }

  emit echo( QString( "# owpl.size() == %1" ).arg(owpl.size()) );
  if ( owpl.size() < 1 ) // Nothing to attack, cut it short for now
    return genmoveEasyD(c);

  // TODO: learn how not to kill the ones that are dead even if you leave them

  qint32 minLib = minimumLiberties( owpl );
  emit echo( QString( "# minLib == %1" ).arg(minLib) );
  if ( minLib < 1 )
    { qDebug( "UNEXPECTED: Sakudo::genmoveKilmer minLib %d", minLib );
      return genmoveEasyD(c);
    }

  if ( minLib == 1 )
    { QList<Wyrm *>potentialKills;
      foreach( Wyrm *wp, owpl )
        if ( wp->libertyList.size() == 1 )
          potentialKills.append(wp);

      // TODO: compare the value of this kill to the value of any available threats
      emit echo( QString( "# potentialKills.size() == %1" ).arg(potentialKills.size()) );
      if ( potentialKills.size() > 0 ) // Only one Wyrm to choose from
        return bp->indexToVertex( largestWyrm( potentialKills )->libertyList.at(0) );
    }

  if ( minLib > 4 )
    return genmoveEasyD(c);

  // Find the most vulnerable opponent Wyrm (relative liberties)
  QList<Wyrm *> twpl = owpl;
  Wyrm *twp = nullptr;
  qint32 libDiff = -2;                // Difference in liberties between opponent Wyrm and its surrounding Wyrms
  while ( twpl.size() > 0 )
    { Wyrm *wp = largestWyrm( twpl ); // Work down from the largest to smallest
      twpl.removeAll( wp );
      QList<Wyrm *>nwpl = surroundingWyrms( wp );
      minLib = minimumLiberties( nwpl );
      if ( minLib < 6 )
        if ( ( minLib - wp->libertyList.size() ) > libDiff )
          { libDiff = minLib - wp->libertyList.size();
            twp = wp;
          }
    }
  emit echo( QString( "# libDiff == %1" ).arg(libDiff) );
  if ( twp != nullptr )
    if ( libDiff >= 0 )
      { // Chosen twp to attack, now choose optimal attack point
        QList<qint32> ail = twp->libertyList;        // Attack Index List
        QList<Wyrm *>nwpl = surroundingWyrms( twp );
        qint32 bestAttackPoint = ail.at(0);
        qint32 leastLibertyImproved = bp->nPoints();
        qint32 bestLibertyResult = 0;
        foreach ( qint32 ap, ail )
          foreach ( Wyrm *wp, nwpl )
            { if ( wp != nullptr )
                if ( wp->libertyList.contains( ap ) )
                  { qint32 tlc = tp->testLibertyCount( ap, c );
                    if ( tlc >= (ail.size() - 1) ) // Could this attack succeed?
                      { if ( leastLibertyImproved == wp->libertyList.size() )
                          if ( bestLibertyResult < tlc )
                            { bestLibertyResult = tlc;
                              bestAttackPoint = ap;
                            }
                        if ( leastLibertyImproved > wp->libertyList.size() )
                          { leastLibertyImproved = wp->libertyList.size();
                            bestLibertyResult = tlc;
                            bestAttackPoint = ap;
                          }
                      }
                  }
            } // foreach ( Wyrm *wp, nwpl )
        if ( tp->testLibertyCount( bestAttackPoint, c ) > 1 ) // Do not self-Atari
          return bp->indexToVertex( bestAttackPoint );
      } // if ( libDiff >= 0 )

  return genmoveEasyD(c);
}

/**
 * @brief Sakudo::surroundingWyrms
 * @param wp - Wyrm to analyze
 * @return list of all other Wyrms that touch wp
 */
QList<Wyrm *> Sakudo::surroundingWyrms( Wyrm *wp )
{ QList<Wyrm *> wpl,iwpl;
  foreach ( Goishi *ip, wp->ipl )
    { iwpl.clear();
      iwpl = surroundingWyrms( bp->xyToIndex(ip->x,ip->y) );
      foreach ( Wyrm *iwp, iwpl )
        if ( iwp != nullptr )
          if ( !wpl.contains( iwp ) )
            wpl.append( iwp );
    }
  return wpl;
}

/**
 * @brief Sakudo::surroundingWyrms
 * @param i - board index point to analyze
 * @return list of all Wyrms that touch i
 */
QList<Wyrm *> Sakudo::surroundingWyrms( qint32 i )
{ QList<Wyrm *> wpl;
  Goishi *ip = bp->goishi(i);
  if ( ip == nullptr )
    return wpl;
  Wyrm *wp = ip->wp; // Wyrm that is being checked for surrounding Wyrms
  if ( wp == nullptr )
    return wpl;
  qint32 x,y;
  bp->indexToXY(i,&x,&y);
  if (x > 0)             { ip = bp->goishiAt(x-1,y); if (ip != nullptr) if (ip->wp != nullptr) if (!wpl.contains(ip->wp)) wpl.append(ip->wp); }
  if (x < bp->Xsize - 1) { ip = bp->goishiAt(x+1,y); if (ip != nullptr) if (ip->wp != nullptr) if (!wpl.contains(ip->wp)) wpl.append(ip->wp); }
  if (y > 0)             { ip = bp->goishiAt(x,y-1); if (ip != nullptr) if (ip->wp != nullptr) if (!wpl.contains(ip->wp)) wpl.append(ip->wp); }
  if (y < bp->Ysize - 1) { ip = bp->goishiAt(x,y+1); if (ip != nullptr) if (ip->wp != nullptr) if (!wpl.contains(ip->wp)) wpl.append(ip->wp); }
  return wpl;
}

/**
 * @brief Sakudo::largestWyrm
 * @param wpl - list of Wyrms to compare
 * @return largest Wyrm in the list
 */
Wyrm *Sakudo::largestWyrm( QList<Wyrm *>wpl )
{ qint32 maxSz = 0;
  Wyrm *maxWyrm = nullptr;
  foreach ( Wyrm *wp, wpl )
    if ( wp != nullptr )
      if ( wp->ipl.size() > maxSz )
        { maxSz = wp->ipl.size();
          maxWyrm = wp;
        }
  return maxWyrm;
}

/**
 * @brief Sakudo::minimumLiberties
 * @param wpl - list of Wyrms to check
 * @return number of liberties of the "most vulnerable" Wyrm in the list
 */
qint32 Sakudo::minimumLiberties( QList<Wyrm *>wpl )
{ qint32 minLib = bp->nPoints();
  foreach ( Wyrm *wp, wpl )
    if ( wp != nullptr )
      if ( wp->libertyList.size() < minLib )
        minLib = wp->libertyList.size();
  return minLib;
}

/**
 * @brief Sakudo::aNotInB
 * @param a - list of numbers to check
 * @param b - list of numbers to check
 * @return true if any number in a is not in b
 */
bool Sakudo::aNotInB( QList<qint32>a, QList<qint32>b )
{ foreach ( qint32 i, a )
    if ( !b.contains(i) )
      return true;
  return false;
}

/**
 * @brief Sakudo::genmoveEasyD - look for easy defense points
 * @param c - color to generate a move for
 * @return desired move
 */
QString Sakudo::genmoveEasyD( qint32 c )
{ emit echo( QString( "# genmoveEasyD(%1)" ).arg(c) );
  if (( tp == nullptr ) || ( bp == nullptr ))           return "pass";
  QList<qint32> lml = tp->allLegalMoves(c);
  emit echo( QString( "# %1 legal moves available" ).arg(lml.size()) );
  if (( lml.size() < 1 ) || allInOwnRyoiki( c, lml ))   return "pass";
  if ( tp->stateHistory.size() <= 1 )                   return firstMove(c);

  QList<Wyrm *>fwpl;  // Friendly Wyrms to try to protect list
  qint32 oc = c+1; if ( oc >= tp->gp->np ) oc = 0; // An opponent color
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveEasyD null Wyrm"); }
       else
        { if ( wp->color() == c )
            if ( canBeAttacked( wp, oc ) )
              if ( wp->libertyList.size() == 1 )
                if ( tp->testLibertyCount( wp->libertyList.at(0), c ) > 1 ) // TODO: evaluate ladders later...
                  fwpl.append(wp);
        }
    }

  emit echo( QString( "# Atari fwpl.size() == %1" ).arg(fwpl.size()) );
  if ( fwpl.size() == 1 )
    return bp->indexToVertex( fwpl.at(0)->libertyList.at(0) );
  if ( fwpl.size() > 1 )
    { Wyrm *wp = largestWyrm( fwpl );
      return bp->indexToVertex( wp->libertyList.at(0) );
    }

  // Trying to convert friendly Wyrms with 2 liberties to 3 or more
  fwpl.clear();
  foreach ( Wyrm *wp, tp->wpl )
    { if ( wp == nullptr )
        { qDebug( "WARNING: Sakudo::genmoveEasyD null Wyrm"); }
       else
        { if ( wp->color() == c )
            if ( canBeAttacked( wp, oc ) )
              if ( wp->libertyList.size() == 2 )
                if (( tp->testLibertyCount( wp->libertyList.at(0), c ) > 2 ) ||
                    ( tp->testLibertyCount( wp->libertyList.at(1), c ) > 2 ))
                  fwpl.append(wp);
        }
    }
  emit echo( QString( "# 2 Lib fwpl.size() == %1" ).arg(fwpl.size()) );
  if ( fwpl.size() > 0 )
    { Wyrm *wp = largestWyrm( fwpl );
      qint32 i = wp->libertyList.at(0);
      if ( tp->testLibertyCount( wp->libertyList.at(0), c ) <
           tp->testLibertyCount( wp->libertyList.at(1), c ) )
        i = wp->libertyList.at(1);
      if ( lml.contains( i ) )
        return bp->indexToVertex( i );
    }

  // Best defense is a good offense, check for opponent Draco with cuttable points
  foreach ( Draco *dp, tp->dpl )
    if ( dp != nullptr )
      if ( dp->color() != c )
        if ( dp->cutPoints.size() > 0 )
          foreach ( qint32 i, dp->cutPoints )
            if ( tp->testLibertyCount(i,c) > 1 ) // Do not move into Atari
              if ( lml.contains( i ) )
                return bp->indexToVertex( i ); // TODO: collect a list of cuttable points and choose the "best one"

  // TODO: more defensive ideas
  return genmoveTerry(c);
}


/**
 * @brief Sakudo::genmoveTerry - return a move, outside of owned Ryoiki
 *   that adds as many Jiyu points as possible, using the Jiyu influence estimator.
 *   Overall, Terry is a terrible and slow player.
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveTerry( qint32 c )
{ emit echo( QString( "# genmoveTerry(%1)" ).arg(c) );
  if (( tp == nullptr ) || ( bp == nullptr )) return "pass";
  QList<qint32> lml = removePassEyes( removeOwnRyoiki( c, tp->allLegalMoves(c) ) );
  foreach ( qint32 i, lml )
    if ( tp->testLibertyCount( i, c ) < 2 )
      lml.removeAll( i );
  emit echo( QString( "# %1 legal non-Atari moves available" ).arg(lml.size()) );
  if ( lml.size() < 1 )                       return "pass";
  if ( tp->stateHistory.size() <= 1 )         return firstMove(c);
  if ( lml.size() == 1 )                      return bp->indexToVertex( lml.at(0) );

  qint32 bestI = -1;
  qreal  bestP = (qreal)(-bp->nPoints());
  foreach ( qint32 i, lml )
    { Game *tgp = new Game( gp, this ); // Test game
      tgp->playGoishiIndex(i,c);
      qreal sc = tgp->tp->jp->score(c);
      if ( sc > bestP )
        { bestP = sc;
          bestI = i;
        }
      tgp->deleteLater();
    }
  if (( bestI >= 0 ) && ( bestI < bp->nPoints() ))
    return bp->indexToVertex( bestI );
  return genmoveRandy(c);
}

/**
 * @brief Sakudo::genmoveMonty - a super simple MonteCarlo move generator (no neural nets, no learning)
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveMonty( qint32 c )
{ if (( tp == nullptr ) || ( bp == nullptr ))
    return "pass";
  QList<qint32> lml = removeOwnRyoiki( c, tp->allLegalMoves(c) );
  if ( lml.size() < 1 )
    return "pass";
  if ( tp->stateHistory.size() <= 1 )
    return firstMove(c);
  if ( lml.size() == 1 )
    return bp->indexToVertex( lml.at(0) );

  QVector<qint32> wins;
  wins.reserve(bp->nPoints());
  while ( wins.size() < bp->nPoints() )
    wins.append(0);

  QTimer timer;
  timer.setSingleShot( true );
  timer.start( 9000 ); // Run a minimum of 9 seconds
  while ( timer.isActive() )
    { foreach ( qint32 i, lml )
        if ( playOneMonty(c,i) )
          wins[i]++;
    }

  qint32 maxWins  = -1;
  qint32 maxIndex = -1;
  for ( qint32 i = 0; i < wins.size(); i++ )
    if ( wins.at(i) > maxWins )
      { maxWins = wins.at(i);
        maxIndex = i;
      }
  if ( maxIndex >= 0 )
    return bp->indexToVertex( maxIndex );
  return genmoveRandy(c);
}

/**
 * @brief Sakudo::playOneMonty - play out a game using genmoveRandy
 *   starting with the current Goban position and color c moving at index i
 * @param c - color to play out a game starting with
 * @param i - next move to play
 * @return true if c won the game
 */
bool Sakudo::playOneMonty( qint32 c, qint32 i )
{ Game *tgp = new Game( gp, this ); // Test game
  tgp->playGoishiIndex(i,c);
  bool result = tgp->mp->finishRandomGame();
  tgp->deleteLater();
  return result;
}

/**
 * @brief Sakudo::finishRandomGame - play it where it lies
 *   generate moves with Randy until two passes result.
 * @return true if initial player wins, false if they lose
 */
bool Sakudo::finishRandomGame()
{ qint32 initialPlayer = gp->pt;
  bool done = false;
  // TODO: play out and determine winner
  return done;
}
