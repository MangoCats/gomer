#include "stones.h"

Stones::Stones(Board *parent) : QObject(parent)
{ size = 0.97826;
  bp = parent;
}

void Stones::clear()
{ foreach( Stone *s, stoneList )
    if ( s )
      delete s;
  stoneList.clear();
}

int Stones::stoneAt( int x, int y )
{ int i = 0;
  while ( i < stoneList.size() )
    { Stone *sp = stoneList.at( i );
      if (( sp->x == x ) && ( sp->y == y ))
        return i;
      i++;
    }
  return -1;
}

Stone * Stones::placeNextStone( int x, int y )
{ if ( stoneAt( x, y ) != -1 )
    return nullptr;
  int i = stoneList.size();
  int color = i & 1; // 0 is black, 1 is white
  Stone *sp = new Stone( x,y,color,i,bp,this );
  stoneList.append( sp );
  return sp;
}

void Stones::clearGroups()
{ foreach( StoneGroup *sgp, groupList )
    if ( sgp != nullptr )
      sgp->deleteLater();
   groupList.clear();
   foreach( Stone *sp, stoneList )
     sp->g = -1;

}

void Stones::computeGroups()
{ clearGroups();
  foreach( Stone *sp, stoneList )
    { if ( bp->isOnBoard( sp ) )
        if ( !isInAGroup( sp ) )  // sp itself is not in a group already
          { Stone *np;
            int g;
            np = sp->northNeighbor();
            g = isInGroup( np );
            if (( g >= 0 ) && ( sp->c == np->c ))
              { groupList.at(g)->addStone( sp );
                sp->g = g;
              }
             else
              { np = sp->southNeighbor();
                g = isInGroup( np );
                if (( g >= 0 ) && ( sp->c == np->c ))
                  { groupList.at(g)->addStone( sp );
                    sp->g = g;
                  }
                 else
                  { np = sp->eastNeighbor();
                    g = isInGroup( np );
                    if (( g >= 0 ) && ( sp->c == np->c ))
                      { groupList.at(g)->addStone( sp );
                        sp->g = g;
                      }
                     else
                      { np = sp->westNeighbor();
                        g = isInGroup( np );
                        if (( g >= 0 ) && ( sp->c == np->c ))
                          { groupList.at(g)->addStone( sp );
                            sp->g = g;
                          }
                         else
                          { sp->g = groupList.size();
                            groupList.append( new StoneGroup( sp, this ) );
                          }
                      }
                  }
              }
          }
    }
}

/**
 * @brief Stones::isInAGroup
 * @param sp
 * @return true if the Stone is currently marked as being a member of a valid group
 */
bool Stones::isInAGroup( Stone *sp )
{ if ( sp == nullptr )
    return false;
  if ( sp->g < 0 )
    return false;
  if ( sp->g >= groupList.size() )
    return false;
  return true;
/*
  foreach ( StoneGroup *sgp, groupList )
    if ( sgp->isInGroup( sp ) )
      return true;
  return false;
*/
}

/**
 * @brief Stones::isInGroup
 * @param sp
 * @return index of the group sp is in, or -1 if sp is in no group
 */
int Stones::isInGroup( Stone *sp )
{ if ( sp == nullptr )
    return -1;
  if ( sp->g < 0 )
    return -1;
  if ( sp->g >= groupList.size() )
    return -1;
  return sp->g;
/*
  if ( groupList.size() <= 0 )
    return -1;
  int i = 0;
  while ( i < groupList.size() )
    { StoneGroup *sgp = groupList.at(i);
      if ( sgp->isInGroup( sp ) )
        return i;
      i++;
    }
  return -1;
*/
}
