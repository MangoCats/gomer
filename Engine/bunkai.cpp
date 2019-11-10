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


  // Replace the existing Kogai with the newly developed one
  if ( ap->op != nullptr )
    ap->op->deleteLater();
  ap->op = op;
}
