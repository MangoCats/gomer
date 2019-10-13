#include "gosu.h"

Gosu::Gosu(Goban *parent) : QObject(parent), bp(parent)
{ qDebug( "Gosu constructor" );
}

void Gosu::addGoishiToBowl(Goishi *p)
{ bowl.append( p ); }
