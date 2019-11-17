#include "bunkai.h"
#include <QDir>
#include <QFile>
#include <QSettings>

Bunkai::Bunkai(Shiko *p) : QObject(p), tp(p)
{ QSettings settings;
  ruikeiFilename = settings.value( "ruikeiFilename", QDir::homePath() + "/Ruikei.dat" ).toString();
  readRuikei();
}

/**
 * @brief Bunkai::readRuikei - de-serialize the Ruikei file into a list of objects in memory
 */
void Bunkai::readRuikei()
{ QFile rf( ruikeiFilename );
  if ( !rf.exists() )
    { qDebug( "Shiko::readRuikei() %s does not exist",qPrintable( ruikeiFilename ) );
      return;
    }
  if ( !rf.open( QIODevice::ReadOnly ) )
    { qDebug( "Shiko::readRuikei() %s could not open for reading",qPrintable( ruikeiFilename ) );
      return;
    }
  QDataStream ds( &rf );
  while ( !ds.atEnd() )
    { Ruikei *ap = new Ruikei(ds,tp);
      if ( ap->isValid() )
        apl.append( ap );
       else
        { ap->deleteLater();
          qDebug( "Shiko::readRuikei() %s problem with file format",qPrintable( ruikeiFilename ) );
          return; // Abort the loop.
        }
    }
}

/**
 * @brief Bunkai::writeRuikei - serialize the Ruikei objects from memoryinto a file
 */
void Bunkai::writeRuikei() const
{ QFile rf( ruikeiFilename );
  if ( !rf.open( QIODevice::WriteOnly ) )
    { qDebug( "Shiko::readRuikei() %s could not open for writing",qPrintable( ruikeiFilename ) );
      return;
    }
  QDataStream ds( &rf );
  foreach ( Ruikei *ap, apl )
    { if ( ap != nullptr )
        { if ( ap->isValid() )
            ap->toDataStream( ds );
           else
            qDebug( "Shiko::writeRuikei() invalid Ruikei encountered" );
        }
       else
        qDebug( "Shiko::writeRuikei() null Ruikei pointer" );
    }
  QSettings settings;
  settings.setValue( "ruikeiFilename", ruikeiFilename );
}

/**
 * @brief Bunkai::matchingRuikei
 * @param ap - Ruikei to try to match
 * @return nullptr if no match is found, or pointer to Ruikei from the list if a match is found
 */
Ruikei *Bunkai::matchingRuikei( Ruikei *ap )
{ foreach ( Ruikei *lap, apl )
    if ( lap->matchPosition( ap ) )
      return lap;
  return nullptr;
}

/**
 * @brief Bunkai::moveOrPass - judge the relative merit of moving in the Ruikei vs passing
 * @param ap - Ruikei to lookup or analyze
 * @return The differential local score between the best possible move or passing
 */
qint32 Bunkai::moveOrPass( Ruikei *ap )
{ if ( ap == nullptr )
    { qDebug( "UNEXPECTED: Bunkai::moveOrPass() received null Ruikei" );
      return 0;
    }
  playout( ap );
  Kogai *op = ap->op;
  if ( op == nullptr )
    { qDebug( "UNEXPECTED: Bunkai::moveOrPass() received null Kogai from playout" );
      return 0;
    }
  return op->highScore - op->passScore;
  // TODO: Extract best score with multiple friendly passes for 2, 3 and maybe more passes.

  // TODO: Condensation of Ruikei with identical Kogai outcomes
  
  // TODO: MonteCarlo Tree Analysis for larger Ruikei

  // TODO? compare new Kogai to the outgoing one...
}

/**
 * @brief Bunkai::playout - recursively play through all legal move combinations
 *   Effectively run a minimax search, each level maximizing the friendly high score
 *   but the meaning of friendly inverting with each level.
 * @param ap - Ruikei to play through
 */
void Bunkai::playout( Ruikei *ap )
{ if ( ap     == nullptr ) { qDebug( "UNEXPECTED: Bunkai::playout null ap"     ); return; }
  if ( ap->op == nullptr ) { qDebug( "UNEXPECTED: Bunkai::playout null ap->op" ); return; }
  qint32 prevMove = MOVE_UNDEFINED_INDEX;
  if ( ap->ap != nullptr )                   // Does this Ruikei include move history?
    { prevMove = ap->previousMove;
      if ( prevMove == MOVE_PASS_INDEX )
        ap->op->passScore = ap->score();
    }
  Ruikei *map = matchingRuikei( ap );        // Looking for Ruikei in the library
  if ( map != nullptr )                      // Precomputed match found?
    ap->op->copy( map->op );
   else
    { Ruikei *sap = nullptr;
      bool legalMoveAvailable = false;
      for ( qint32 i = (prevMove == MOVE_PASS_INDEX) ? 0 : MOVE_PASS_INDEX; i < ap->nPoints(); i++ )
        { Ruikei *nap = new Ruikei( ap, i ); // Next analysis position
          if ( !nap->isValid() )             // Illegal move?
            { delete nap; }
           else
            { legalMoveAvailable = true;
              playout( nap );
              if ( i == MOVE_PASS_INDEX )    // Scores invert because next level is inverted
                ap->op->passScore = -nap->op->highScore;
              if ( ap->op->highScore < -nap->op->highScore )
                { ap->op->highScore = -nap->op->highScore;
                  ap->op->bestMove = i;
                  if ( ap->ap == nullptr )   // Are we at depth 0?
                    { if ( sap != nullptr )
                        delete sap;
                      sap = nap;
                    }
                } // if new high score
            } // else: move at i is valid
          if ( nap != sap )
            delete nap;
        } // for ( i = ...) try all moves
      if ( sap != nullptr )
        apl.append( sap );                   // Building up the library with selected observed and computed scenarios
      if ( !legalMoveAvailable )             // Note: pass is considered as a legal move if previous move was not also pass..
        { ap->op->highScore =
          ap->op->passScore = ap->score();   // No more moves available, the score is the score
        }
    } // else: precomputed match not found, compute results
}
