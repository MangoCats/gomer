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
  // Play again with opponent passing once, twice, three times - until maybe 6 passes, or result is the same
  // Play again with friendly passing once, twice, etc. as with opponent.
  // Store the optimal and N-pass results each in their own Soshi in the Kogai
  //   also, reach back into the Shiko and add Ruikei - at least for the optimal paths, to the Shiko's Ruikei list
  for ( qint32 i = 0; i < ap->nPoints(); i++ )
    playout( ap, i );
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
 * @return true if at least one step was played at this point
 */
bool  Bunkai::playout( Ruikei *ap, qint32 i )
{ Ruikei *nap = new Ruikei( ap, i );
  if ( !nap->isValid() )
    { nap->deleteLater();
      return false;
    }
  // playout all possible children
  for ( qint32 i = 0; i < nap->nPoints(); i++ )
    playout( nap, i );
  return true;
}
