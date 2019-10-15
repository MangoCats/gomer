#include "gosu.h"

Gosu::Gosu(Goban *parent) : QObject(parent), bp(parent)
{ qDebug( "Gosu constructor" );
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
  emit bowlCountChanged( bowl.size() );
}

/**
 * @brief Gosu::addGoishiToLid
 * @param ip - pointer to a Goishi that is being placed in the lid
 */
void Gosu::addGoishiToLid(Goishi *ip)
{ lid.append( ip );
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

