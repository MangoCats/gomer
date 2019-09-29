#include "gamecontrol.h"

GameControl::GameControl(QObject *parent) : QObject(parent)
{

}

void GameControl::start()
{ emit stateMessage( "Black to Move" ); }

void GameControl::clickAt( QPointF p )
{ QString msg = QString("gc clickAt %1 %2").arg( p.rx() ).arg( p.ry() );
  qDebug( qPrintable( msg ) );
}
