#include "gamecontrol.h"
#include <cmath>

GameControl::GameControl( Board *pbp, QObject *parent) : QObject(parent)
{ bp = pbp; }

void GameControl::start()
{ emit stateMessage( "Black to Move" ); }

qreal GameControl::stoneSize()
{ if ( bp )
    return bp->stoneSize();
  return 0.618;
}

void GameControl::clickAt( QPointF p )
{ QString msg = QString("gc clickAt %1 %2").arg( p.rx() ).arg( p.ry() );
  qDebug( qPrintable( msg ) );
  if (( p.rx() < -0.5 ) ||
      ( p.ry() < -0.5 ) ||
      ( p.rx() > ((qreal)bp->Xsize) - 0.5 ) ||
      ( p.ry() > ((qreal)bp->Xsize) - 0.5 ))
    { qDebug( "out of bounds" );
      return;
    }
  int nearX = (int)(p.rx() + 0.5);
  int nearY = (int)(p.ry() + 0.5);
  qreal distX = p.rx() - (qreal)nearX;
  qreal distY = p.ry() - (qreal)nearY;
  qreal dist = sqrt( distX*distX + distY*distY );
  if ( dist > stoneSize()/2.0 )
    { qDebug( "ambiguous" );
      return;
    }
  msg = QString("gc clickAt %1 %2").arg( nearX ).arg( nearY );
  qDebug( qPrintable( msg ) );
  proposeMove( nearX, nearY );
}


void GameControl::proposeMove( int x, int y )
{ QString msg;
  if ( !bp )
    { qDebug( "game board undefined" );
      return;
    }
  if (( x < 0 ) ||
      ( y < 0 ) ||
      ( x >= bp->Xsize ) ||
      ( y >= bp->Ysize ))
    { qDebug( "out of bounds" );
      return;
    }
  if ( bp->stoneIndexAt( x, y ) >= 0 )
    { msg = QString( "Stone already present at %1,%2" ).arg( x ).arg( y );
      qDebug( qPrintable( msg ) );
      return;
    }
  Stone *sp = bp->placeNextStone( x, y );
  if ( sp != nullptr )
    { // trigger drawing of stone
      int c = sp->c;
      emit newStonePlaced( sp );
      if ( c == 0 )
        emit stateMessage( "White to Move" );
       else
        emit stateMessage( "Black to Move" );
    }
}

void GameControl::hover( QPointF p )
{ QString msg;
  if ( !bp )
    return;
  if (( p.rx() < -0.5 ) ||
      ( p.ry() < -0.5 ) ||
      ( p.rx() > ((qreal)bp->Xsize) - 0.5 ) ||
      ( p.ry() > ((qreal)bp->Ysize) - 0.5 ))
    return;
  static int lastX = -1;
  static int lastY = -1;
  int nearX = (int)(p.rx() + 0.5);
  int nearY = (int)(p.ry() + 0.5);
  if (( nearX == lastX ) && ( nearY == lastY ))
    return;
  lastX = nearX;
  lastY = nearY;
  if (( nearX < 0 ) ||
      ( nearY < 0 ) ||
      ( nearX >= bp->Xsize ) ||
      ( nearY >= bp->Ysize ))
    return;
  Stone *sp = bp->stoneAt( nearX, nearY );
  if ( !sp )
    { msg = QString("%1 %2 empty").arg( nearX ).arg( nearY );
      qDebug( qPrintable( msg ) );
      return;
    }
  if ( !bp->stones )
    return;
  if ( bp->stones->groupList.size() <= sp->g )
    return;
  msg = QString( "x%1 y%2 c%3 lc%4 g%5" ).arg(nearX).arg(nearY).arg( sp->c )
                                .arg( bp->stones->groupList.at( sp->g )->libertyCount() )
                                .arg( sp->g );
  qDebug( qPrintable( msg ) );
}
