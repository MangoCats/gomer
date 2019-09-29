#include "board.h"

Board::Board( QObject *parent ) : QObject(parent)
{ Xsize = Ysize = 19;
  stones = new Stones( this );
}

Board::~Board()
{ if ( stones )
    delete stones;
}

qreal Board::stoneSize()
{ if ( stones )
    return stones->size;
  return 0.618;
}

/**
 * @brief Board::stoneAt
 * @param x
 * @param y
 * @return index of the stone at x,y or -1 if there is no stone there, or -2 if x,y are illegal coordinates, or -3 if there is an internal error
 */
int Board::stoneAt( int x, int y )
{ if (( x < 0 ) ||
      ( y < 0 ) ||
      ( x >= Xsize ) ||
      ( y >= Ysize ))
    return -2;
  if ( !stones )
    return -3;
  return stones->stoneAt( x, y );
}

bool Board::placeNextStone( int x, int y )
{ if ( !stones )
    return false;
  return stones->placeNextStone( x, y );
}

