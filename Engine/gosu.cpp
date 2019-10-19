#include "gosu.h"

Gosu::Gosu(Goban *parent) : QObject(parent), bp(parent)
{ // qDebug( "Gosu constructor" );
}

/**
 * @brief Gosu::Gosu - copy constructor
 * @param sp - Gosu to copy
 * @param parent - parent of the new Gosu
 */
Gosu::Gosu(Gosu *sp, Goban *parent) : QObject(parent), bp(parent)
{ foreach( Goishi *ip, sp->bowl )
    bowl.append( new Goishi(ip,bp) );
  foreach( Goishi *ip, sp->lid )
    lid .append( new Goishi(ip,bp) );
}

/**
 * @brief Gosu::empty - delete all Goishi contained in the bowl and lid
 */
void Gosu::empty()
{ foreach ( Goishi *ip, bowl )
    ip->deleteLater();
  bowl.clear();
  foreach ( Goishi *ip, lid )
    ip->deleteLater();
  lid.clear();
}

/**
 * @brief Gosu::addGoishiToBowl
 * @param ip - pointer to a Goishi that is being placed in the bowl
 */
void Gosu::addGoishiToBowl(Goishi *ip)
{ bowl.append( ip );
  ip->x = GOSU_BOWL; // Notional position of the bowl, off the board
  ip->y = ip->color;
  emit bowlCountChanged( bowl.size() );
}

/**
 * @brief Gosu::addGoishiToLid
 * @param ip - pointer to a Goishi that is being placed in the lid
 */
void Gosu::addGoishiToLid(Goishi *ip)
{ lid.append( ip );
  ip->x = GOSU_LID;  // Notional position of the lid, off the board
  if ( bowl.size() > 0 )
    ip->y = bowl.at(0)->color;
   else
    ip->y = 1 - ip->color; // Weird, and unlikely case, needs attention if playing more than 2 colors and running out of Goishi in the bowl
  emit lidCountChanged( lid.size() );
}

/**
 * @brief Gosu::takeGoishiFromBowl
 * @return a Goishi pointer from the bowl, or nullptr if the bowl is empty
 */
Goishi *Gosu::takeGoishiFromBowl()
{ if ( bowl.size() <= 0 )
    return nullptr;
  Goishi *ip = bowl.takeLast();
  emit bowlCountChanged( bowl.size() );
  return ip;
}

/**
 * @brief Gosu::takeGoishiFromLid
 * @return a Goishi pointer from the lid, or nullptr if the lid is empty
 */
Goishi *Gosu::takeGoishiFromLid()
{ if ( lid.size() <= 0 )
    return nullptr;
  Goishi *ip = lid.takeLast();
  emit lidCountChanged( lid.size() );
  return ip;
}

