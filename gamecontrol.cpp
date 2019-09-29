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
}
