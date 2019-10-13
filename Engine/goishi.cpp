#include "goishi.h"

Goishi::Goishi(qint32 c, Goban *parent) : QObject(parent), bp(parent), color(c)
{ // qDebug( "Goishi constructor %d", color );

}

