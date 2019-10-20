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
    { case 0: return genmoveRandy(c);

      default:
      case 1: return genmoveKilroy(c);
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
 * @param c - color to generate a move for
 * @return proposed move as a vertex, or pass or resign
 */
QString Sakudo::genmoveKilroy( qint32 c )
{ return "pass";

}
