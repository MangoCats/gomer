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

/**
 * @brief Stones::stoneAt - scan all of stoneList looking for a stone at x,y
 * @param x - grid location to check
 * @param y - grid location to check
 * @return index in stoneList of the stone at x,y - or -1 if there is no stone in stoneList at x,y
 */
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

/**
 * @brief Stones::placeNextStone
 * @param x - where to place the stone
 * @param y - where to place the stone
 * @return pointer to the stone that was placed, or null if there was already a stone there
 */
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
          { sp->g = groupList.size();
            groupList.append( new StoneGroup( sp, this ) );
            // Floodfill
            sp->inviteNeighborToGroup( sp->northNeighbor() );
            sp->inviteNeighborToGroup( sp->southNeighbor() );
            sp->inviteNeighborToGroup( sp->eastNeighbor()  );
            sp->inviteNeighborToGroup( sp->westNeighbor()  );
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
    { qDebug( "ERROR: sp->g >= groupList.size()" );
      return false;
    }
  return true;
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
}

/**
 * @brief StoneGroup::atariList - return a list of all points which may be played to capture a group of the passed color
 * @param color - only list Ataris for groups of this color
 * @return  A list of the Atari (capture) points for the passed color
 */
QList<QPoint> Stones::atariList( int color )
{ QList<QPoint> al;
  foreach( StoneGroup *sgp, groupList )
    { if ( sgp != nullptr )
        if ( sgp->group.size() > 0 )
          if ( sgp->group.at(0)->c == color )
            { QList<QPoint> ll = sgp->libertyList();
              if ( ll.size() == 1 )
                al.append( ll.at(0) );
            }
    }
  return al;
}

