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

bool Stones::placeNextStone( int x, int y )
{ if ( stoneAt( x, y ) != -1 )
    return false;
  int color = stoneList.size() & 1; // 0 is black, 1 is white
  stoneList.append( new Stone( x,y,color,bp,this ) );
  return true;
}
