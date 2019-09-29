#include "board.h"

Board::Board( QObject *parent ) : QObject(parent)
{ Xsize = Ysize = 19;
  stones = new Stones( this );
  for ( int i = 0; i < Xsize; i++ )
    for ( int j = 0; j < Ysize; j++ )
      board.append( nullptr );
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
int Board::stoneIndexAt( int x, int y )
{ if ( !isOnBoard( x, y ) )
    return -2;
  if ( !stones )
    return -3;
  return stones->stoneAt( x, y );
}

Stone *Board::stoneAt( int x, int y )
{ if ( !isOnBoard( x, y ) )
    return nullptr;
  if ( !stones )
    return nullptr;
  return board.at( x + y * Xsize );
}


bool Board::placeNextStone( int x, int y )
{ if ( !stones )
    return false;
  if ( !legalMove( x, y ) )
    return false;
  Stone * sp = stones->placeNextStone( x, y );
  if ( sp == nullptr )
    return false;
  board.replace( x + y * Xsize, sp );
  stones->computeGroups();
  return true;
}

bool Board::isOnBoard( Stone *sp )
{ if ( sp == nullptr )
    return false;
  return isOnBoard( sp->x, sp->y );
}

bool Board::isOnBoard( int x, int y )
{ if (( x < 0 ) ||
      ( y < 0 ) ||
      ( x >= Xsize ) ||
      ( y >= Ysize ))
    return false;
  return true;
}

bool Board::legalMove( int x, int y )
{ if ( !isOnBoard( x, y ) )
    return false;
  if ( stoneAt( x, y ) != nullptr )
    return false;
  // TODO: check for self capture
  // TODO: check for Ko
  return true;
}
