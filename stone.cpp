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
  return board->stoneAt( x, y - 1 );
}

Stone * Stone::southNeighbor()
{ if ( board == nullptr )
    return nullptr;
  return board->stoneAt( x, y + 1 );
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
 * @brief Stone::liberties
 * @return the number of liberties this individual stone contributes to its group
 */
int Stone::liberties()
{ int lc = 0;
  if ( y > 0 )
    if ( northNeighbor() == nullptr )
      lc++;
  if ( y < (board->Ysize - 1) )
    if ( southNeighbor() == nullptr )
      lc++;
  if ( x > 0 )
    if ( eastNeighbor() == nullptr )
      lc++;
  if ( x < (board->Xsize - 1) )
    if ( westNeighbor() == nullptr )
      lc++;
  return lc;
}
