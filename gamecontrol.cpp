#include "gamecontrol.h"

GameControl::GameControl(QObject *parent) : QObject(parent)
{

}

void GameControl::start()
{ emit stateMessage( "Black to Move" ); }
