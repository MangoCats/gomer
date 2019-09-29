#include "stone.h"

Stone::Stone( int px, int py, int pc, int pi, Board *pb, QObject *parent) : QObject(parent)
{ x = px;
  y = py;
  c = pc;
  i = pi;
  g = -1;
  board = pb;
}

Stone * Stone::northNeighbor()
{ if ( board == nullptr )
    return nullptr;
  return board->stoneAt( x, y + 1 );
}

Stone * Stone::southNeighbor()
{ if ( board == nullptr )
    return nullptr;
  return board->stoneAt( x, y - 1 );
}

Stone * Stone::eastNeighbor()
{ if ( board == nullptr )
    return nullptr;
  return board->stoneAt( x - 1, y );
}

Stone * Stone::westNeighbor()
{ if ( board == nullptr )
    return nullptr;
  return board->stoneAt( x + 1, y );
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
