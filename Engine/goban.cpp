#include "goban.h"

Goban::Goban(QObject *parent,qint32 xs,qint32 ys) : QObject(parent), Xsize(xs), Ysize(ys)
{ qDebug( "Goban constructor %d x %d", Xsize, Ysize );

}
