#include "bunkai.h"

Bunkai::Bunkai(QObject *parent) : QObject(parent)
{}

/**
 * @brief Bunkai::predictTerritory
 * @param ap - Ruikei to make a new Kogai for
 */
void  Bunkai::predictTerritory( Ruikei *ap )
{ if ( ap == nullptr )
    { qDebug( "UNEXPECTED: Bunkai::predictTerritory() received null Ruikei" );
      return;
    }
  Kogai *op = new Kogai( ap ); // For now, work on a new Kogai
  // TODO: game it forward, play every legal move, for each of those play every legal response, etc. record the best result for each turn:
  // In other words: Friendly turn, which one gets the best result when Opponent turn gets their best result when Friendly turn gets their best result, etc.
  // Store the optimal and N-pass results each in their own Soshi in the Kogai
  //   also, reach back into the Shiko and add Ruikei - at least for the optimal paths, to the Shiko's Ruikei list
  qint32 passScore = SCORE_INVALID_MOVE;
  qint32 highScore = SCORE_INVALID_MOVE;
  qint32 bestMove  = MOVE_PASS_INDEX;
  for ( qint32 i = MOVE_PASS_INDEX; i < ap->nPoints(); i++ )
    { qint32 iScore = playout( ap, i );
      if ( iScore > highScore )
        { highScore = iScore;
          bestMove  = i;
        }
      if ( i == MOVE_PASS_INDEX )
        passScore = iScore;
    }
  // TODO: Condensation of Ruikei with identical Kogai outcomes
  
  // TODO: MonteCarlo Tree Analysis for larger Ruikei

  // TODO? compare new Kogai to the outgoing one...
  
  // Replace the existing Kogai with the newly developed one
  if ( ap->op != nullptr )
    ap->op->deleteLater();
  ap->op = op;
}

/**
 * @brief Bunkai::playout - recursively play through all combinations
 * @param ap - Ruikei to play through
 * @param i - particular grid point to play this iteration
 * @return final score of the chosen branch
 * // TODO: track multi-dimensional comparison of best possible score vs best score with 1 friendly pass, 2 friendly passes, etc.
 */
qint32 Bunkai::playout( Ruikei *ap, qint32 i )
{ Ruikei *nap = new Ruikei( ap, i );
  if ( !nap->isValid() )
    { nap->deleteLater();
      return ap->score();  // End of the line, return the actual score
    }
  // Playout (most) all possible children, but not pass if previous move was pass
  qint32 highScore = SCORE_INVALID_MOVE;
  for ( qint32 ni = (i == MOVE_PASS_INDEX) ? 0 : MOVE_PASS_INDEX; ni < nap->nPoints(); ni++ )
    { qint32 niScore = playout( nap, ni );
      if ( niScore > highScore )
        highScore = niScore;
    }
  return highScore;  // Best score of all child positions, assuming that minimax search path is what will be followed
}
