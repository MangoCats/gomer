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

/**
 * @brief Board::legalMove - would placing the next stone at x,y be legal?
 * @param x
 * @param y
 * @return true if the move would be legal
 */
bool Board::legalMove( int x, int y )
{ if ( !stones )
    return false;
  if ( !isOnBoard( x, y ) )
    return false;
  if ( stoneAt( x, y ) != nullptr )
    return false;
  if ( selfCapture( x, y, colorToMove() ) )
    return false;
  // TODO: check for Ko
  return true;
}

/**
 * @brief Board::colorToMove
 * @return 0 if black moves next, 1 if white
 */
int  Board::colorToMove()
{ if ( stones )
    return stones->colorToMove();
  return 0;
}

/**
 * @brief Board::selfCapture - is the proposed stone placement a self-caputre?
 * @param x - stone placement under consideration
 * @param y - stone placement under consideration
 * @param c - color being considered to place at x, y
 * @return true if placing a c stone at x,y would be an illegal self-capture.
 */
bool Board::selfCapture( int x, int y, int c )
{ if ( !stones )
    { qDebug( "ERROR: stones not defined when computing selfCapture" );
      return true;
    }
  // Check for open neighbors
  if ( x > 0 )
    if ( stoneAt( x-1,y ) == nullptr )
      return false;
  if ( x <= (Xsize - 1) )
    if ( stoneAt( x+1,y ) == nullptr )
      return false;
  if ( y > 0 )
    if ( stoneAt( x,y-1 ) == nullptr )
      return false;
  if ( y <= (Ysize - 1) )
    if ( stoneAt( x,y+1 ) == nullptr )
      return false;

  // Check for capture of enemy
  QList<QPoint> al = stones->atariList( c );
  foreach( QPoint p, al )
    if (( p.x() == x ) && ( p.y() == y ))
      return false;

  // Check for friendly neighbors with excess liberties
  Stone *sp;
  int gls = stones->groupList.size();
  if ( x > 0 )
    { sp = stoneAt( x-1,y );
      if ( sp->c == c )
        { if ( sp->g >= gls )
            { qDebug( "ERROR: unexpected sp->g >= stones->groupList.size()" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->c != c )
            { qDebug( "ERROR: stones->groupList.at( sp->g )->c != c" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->libertyCount() > 1 )
            return false; // Can't be self-capture if this adjacent friendly group has 2 or more liberties
        }
    }

  if ( x <= ( Xsize - 1 ) )
    { sp = stoneAt( x+1,y );
      if ( sp->c == c )
        { if ( sp->g >= gls )
            { qDebug( "ERROR: unexpected sp->g >= stones->groupList.size()" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->c != c )
            { qDebug( "ERROR: stones->groupList.at( sp->g )->c != c" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->libertyCount() > 1 )
            return false; // Can't be self-capture if this adjacent friendly group has 2 or more liberties
        }
    }

  if ( y > 0 )
    { sp = stoneAt( x,y-1 );
      if ( sp->c == c )
        { if ( sp->g >= gls )
            { qDebug( "ERROR: unexpected sp->g >= stones->groupList.size()" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->c != c )
            { qDebug( "ERROR: stones->groupList.at( sp->g )->c != c" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->libertyCount() > 1 )
            return false; // Can't be self-capture if this adjacent friendly group has 2 or more liberties
        }
    }

  if ( y <= ( Ysize - 1 ) )
    { sp = stoneAt( x,y+1 );
      if ( sp->c == c )
        { if ( sp->g >= gls )
            { qDebug( "ERROR: unexpected sp->g >= stones->groupList.size()" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->c != c )
            { qDebug( "ERROR: stones->groupList.at( sp->g )->c != c" );
              return true;
            }
          if ( stones->groupList.at( sp->g )->libertyCount() > 1 )
            return false; // Can't be self-capture if this adjacent friendly group has 2 or more liberties
        }
    }

  // TODO: the expensive calculation with a prospective board....

  return true;
}
