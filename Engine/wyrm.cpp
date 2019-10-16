#include "wyrm.h"

/**
 * @brief Wyrm::Wyrm
 * @param ipp - single Goishi Wyrm
 * @param parent
 */
Wyrm::Wyrm(Goishi *ipp, Shiko *parent) : QObject(parent)
{ addGoishi( ipp ); }

/**
 * @brief Wyrm::addGoishi - add one Goishi to this Wyrm
 * @param ip
 */
void Wyrm::addGoishi( Goishi *ip )
{ if ( ipl.size() > 0 )
    if ( ipl.at(0)->color != ip->color )
      { qDebug( "Wyrm::addGoishi color mismatch %d %d", ipl.at(0)->color, ip->color );
        return;
      }
  ipl.append( ip );
  ip->wp = this;
}

/**
 * @brief Wyrm::addWyrm - add the passed Wyrm's Goishi to this Wyrm
 * @param wp - Wyrm to add
 */
void Wyrm::addWyrm( Wyrm *wp )
{ foreach ( Goishi *ip, wp->ipl )
    addGoishi( ip );
}

qint32 Wyrm::color()
{ if ( ipl.size() <= 0 )
    return -1;
  return ipl.at(0)->color;
}
