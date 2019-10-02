#include "stone.h"

Stone::Stone( int px, int py, int pc, int pi, Board *pb, QObject *parent) : QObject(parent)
{ ei = nullptr;
  x  = px;
  y  = py;
  c  = pc;
  i  = pi;
  g  = -1;
  board = pb;
}

Stone * Stone::northNeighbor()
{ if ( board == nullptr )
    return nullptr;
  if ( y >= ( board->Ysize - 1 ) )
    return nullptr;
  return board->stoneAt( x, y + 1 );
}

Stone * Stone::southNeighbor()
{ if ( board == nullptr )
    return nullptr;
  if ( y <= 0 )
    return nullptr;
  return board->stoneAt( x, y - 1 );
}

Stone * Stone::eastNeighbor()
{ if ( board == nullptr )
    return nullptr;
  if ( x <= 0 )
    return nullptr;
  return board->stoneAt( x - 1, y );
}

Stone * Stone::westNeighbor()
{ if ( board == nullptr )
    return nullptr;
  if ( x >= ( board->Xsize - 1 ) )
    return nullptr;
  return board->stoneAt( x + 1, y );
}

/**
 * @brief Stone::inviteNeighborToGroup - this stone is looking for neighbor groups
 * @param np - neighbor to join, if that's a valid thing to do
 */
void  Stone::inviteNeighborToGroup( Stone *np )
{ if ( np == nullptr ) // empty grid, can't join that
    return;
  if ( np->c != c ) // wrong color, can't join that
    return;
  if ( g < 0 ) // not in a group, can't invite neighbors
    { qDebug( "ERROR: inviting neighbor to group when not a member of a group yet, shouldn't happen" );
      return;
    }
  if ( np->g >= 0 ) // already member of a group
    { if ( np->g != g )
        qDebug( "ERROR: neighbor is a member of a different group, shouldn't be..." );
      return;
    }
  np->g = g; // Neighbor joining our group
  board->stones->groupList.at( g )->group.append( np );
  // Now, invite their neighbors
  inviteNeighborToGroup( np->northNeighbor() );
  inviteNeighborToGroup( np->southNeighbor() );
  inviteNeighborToGroup( np->eastNeighbor()  );
  inviteNeighborToGroup( np->westNeighbor()  );
}


/**
 * @brief Stone::libertyCount
 * @return the number of liberties this individual stone contributes to its group
 */
int Stone::libertyCount()
{ int lc = 0;
  if ( y > 0 )
    if ( southNeighbor() == nullptr )
      lc++;
  if ( y < (board->Ysize - 1) )
    if ( northNeighbor() == nullptr )
      lc++;
  if ( x > 0 )
    if ( eastNeighbor() == nullptr )
      lc++;
  if ( x < (board->Xsize - 1) )
    if ( westNeighbor() == nullptr )
      lc++;
  return lc;
}

QList<QPoint> Stone::libertyList()
{ QList<QPoint> ll;
  if ( y > 0 )
    if ( southNeighbor() == nullptr )
      ll.append( QPoint(x,y-1) );
  if ( y < (board->Ysize - 1) )
    if ( northNeighbor() == nullptr )
      ll.append( QPoint(x,y+1) );
  if ( x > 0 )
    if ( eastNeighbor() == nullptr )
      ll.append( QPoint(x-1,y) );
  if ( x < (board->Xsize - 1) )
    if ( westNeighbor() == nullptr )
      ll.append( QPoint(x+1,y) );
  return ll;
}
