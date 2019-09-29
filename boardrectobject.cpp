#include "boardrectobject.h"

BoardRectObject::BoardRectObject(QObject *parent) : QObject(parent)
{}

void BoardRectObject::clickAt( QPointF p )
{ emit clicked(p); }
