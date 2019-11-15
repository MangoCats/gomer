#include "kigo.h"

Kigo::Kigo(QObject *p) : QObject(p)
{              emptyGrid = true;
          friendlyGoishi = false;
          opponentGoishi = false;
  outsideFriendlyLiberty = false;
  outsideOpponentLiberty = false;
                      ko = false;
                   wyrm1 = false;
                   wyrm2 = false;
}

Kigo::Kigo(const Kigo&p) : QObject(p.parent())
{              emptyGrid = p.emptyGrid;
          friendlyGoishi = p.friendlyGoishi;
          opponentGoishi = p.opponentGoishi;
  outsideFriendlyLiberty = p.outsideFriendlyLiberty;
  outsideOpponentLiberty = p.outsideOpponentLiberty;
                      ko = p.ko;
                   wyrm1 = p.wyrm1;
                   wyrm2 = p.wyrm2;
}

void Kigo::toDataStream( QDataStream &ds ) const
{ ds << toByte(); }

quint8 Kigo::toByte() const
{ return
    (      emptyGrid         ? 0x01 : 0x0) |
    ( friendlyGoishi         ? 0x02 : 0x0) |
    ( opponentGoishi         ? 0x04 : 0x0) |
    ( outsideFriendlyLiberty ? 0x08 : 0x0) |
    ( outsideOpponentLiberty ? 0x10 : 0x0) |
    (                     ko ? 0x20 : 0x0) |
    (                  wyrm1 ? 0x40 : 0x0) |
    (                  wyrm2 ? 0x80 : 0x0);
}

bool Kigo::fromDataStream( QDataStream &ds )
{ quint8 b;
  ds >> b;
  return fromByte( b );
}

bool Kigo::fromByte( quint8 b )
{              emptyGrid = ((b & 0x01) != 0);
          friendlyGoishi = ((b & 0x02) != 0);
          opponentGoishi = ((b & 0x04) != 0);
  outsideFriendlyLiberty = ((b & 0x08) != 0);
  outsideOpponentLiberty = ((b & 0x10) != 0);
                      ko = ((b & 0x20) != 0);
                   wyrm1 = ((b & 0x40) != 0);
                   wyrm2 = ((b & 0x80) != 0);
  return isValid();
}

bool Kigo::operator !=( const Kigo &kc ) const
{ if ( kc.emptyGrid              != emptyGrid              ) return false;
  if ( kc.friendlyGoishi         != friendlyGoishi         ) return false;
  if ( kc.opponentGoishi         != opponentGoishi         ) return false;
  if ( kc.outsideFriendlyLiberty != outsideFriendlyLiberty ) return false;
  if ( kc.outsideOpponentLiberty != outsideOpponentLiberty ) return false;
  if ( kc.ko                     != ko                     ) return false;
  // TODO: more thorough analysis when all aspects are defined
  return true;
}

/**
 * @brief Kigo::invert - swap sides, as is done during minimax turn advance
 */
void Kigo::invert()
{ bool              temp = friendlyGoishi;
          friendlyGoishi = opponentGoishi;
          opponentGoishi = temp;
                    temp = outsideFriendlyLiberty;
  outsideFriendlyLiberty = outsideOpponentLiberty;
  outsideOpponentLiberty = temp;
                      ko = false;
}

/**
 * @brief Kigo::isValid
 * @return false if there are obvious problems with the state values
 */
bool Kigo::isValid() const
{ if ( ko ) // ko must coexist with an empty grid, only
    { if ( !emptyGrid     ||
           friendlyGoishi ||
           opponentGoishi ||
           wyrm1          ||
           wyrm2            )
        return false;
      return true;
    }
  if ( outsideFriendlyLiberty && !friendlyGoishi ) return false;
  if ( outsideOpponentLiberty && !opponentGoishi ) return false;
  // Other combinations are legal (don't cares)
  return true;
}

/**
 * @brief Kigo::isTerritory
 * @param friendly - checking for friendly or opponent territory
 * @return true if the checked for territory matches this Kigo's territory state
 */
bool Kigo::isTerritory( bool friendly ) const
{ // TODO: actually compute territory for Kigos
  return false;
}
