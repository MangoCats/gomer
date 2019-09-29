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
