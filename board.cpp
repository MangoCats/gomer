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

Stone *Board::placeNextStone( int x, int y )
{ if ( !stones )
    return nullptr;
  if ( !legalMove( x, y ) )
    return nullptr;
  Stone * sp = stones->placeNextStone( x, y );
  if ( sp == nullptr )
    { qDebug( "stones->placeNextStone( x, y ) == nullptr" );
      return sp;
    }
  executeCaptures( x,y,sp->c );
  board.replace( x + y * Xsize, sp );
  stones->computeGroups();
  return sp;
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
 * @brief Board::executeCaptures - remove captured stones from the board
 * @param x - where new stone is being placed
 * @param y - where new stone is being placed
 * @param c - color of new stone
 * @return number of stones captured
 */
int Board::executeCaptures( int x, int y, int c )
{ int n = 0;
  if ( stones == nullptr )
    { qDebug( "ERROR: stones is NULL" );
      return -1;
    }
  foreach( StoneGroup *sgp, stones->groupList )
    { if ( sgp != nullptr )
        if ( sgp->group.size() > 0 )
          if ( sgp->group.at(0)->c != c )
            { QList<QPoint> ll = sgp->libertyList();
              if ( ll.size() == 1 )  // Atari?
                if (( ll.at(0).x() == x ) &&
                    ( ll.at(0).y() == y )) // Capturing this Atari?
                  { foreach( Stone *sp, sgp->group )
                      { emit captured( sp );
                        board.replace( sp->x + sp->y * Xsize, nullptr );
                        sp->x = -2;
                        sp->y = c;
                        n++;
                      }
                  }
            }
    }
  return n;
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

  // Check for capture of opponent
  QList<QPoint> al = stones->atariList( 1 - c ); // Looking for opponent's ataris
  foreach( QPoint p, al )
    if (( p.x() == x ) && ( p.y() == y ))
      return false; // any opponent capture is enough to guarantee this is not a self capture

  // Check for friendly neighbors with excess liberties
  int flc;
  if ( x > 0 )
    { flc = friendlyLibertyCount( x-1,y,c );
      if ( flc < 0 ) // error
        return true;
      if ( flc > 1 )
        return false;  // Can't be self-capture if the adjacent friendly group has 2 or more liberties
    }

  if ( x <= ( Xsize - 1 ) )
    { flc = friendlyLibertyCount( x+1,y,c );
      if ( flc < 0 ) // error
        return true;
      if ( flc > 1 )
        return false;  // Can't be self-capture if the adjacent friendly group has 2 or more liberties
    }

  if ( y > 0 )
    { flc = friendlyLibertyCount( x,y-1,c );
      if ( flc < 0 ) // error
        return true;
      if ( flc > 1 )
        return false;  // Can't be self-capture if the adjacent friendly group has 2 or more liberties
    }

  if ( y <= ( Ysize - 1 ) )
    { flc = friendlyLibertyCount( x,y+1,c );
      if ( flc < 0 ) // error
        return true;
      if ( flc > 1 )
        return false;  // Can't be self-capture if the adjacent friendly group has 2 or more liberties
    }

  // TODO: the expensive calculation with a prospective board....

  return true;
}

/**
 * @brief Board::friendlyLibertyCount
 * @param x
 * @param y
 * @param c - color
 * @return libertyCount of stone at the passed position, if color matches, 0 if color does not match, 1 if grid is empty, or -1 if there is an error
 */
int Board::friendlyLibertyCount( int x, int y, int c )
{ Stone *sp = stoneAt( x,y );
  if ( sp == nullptr )
    return 1;
  if ( sp->c != c )
    return 0;
  if ( sp->g >= stones->groupList.size() )
    { qDebug( "ERROR: unexpected sp->g >= stones->groupList.size()" );
      return -1;
    }
  if ( stones->groupList.at( sp->g )->c != c )
    { qDebug( "ERROR: stones->groupList.at( sp->g )->c != c" );
      return -1;
    }
  return stones->groupList.at( sp->g )->libertyCount();
}
