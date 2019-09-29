#include "stones.h"

Stones::Stones(QObject *parent) : QObject(parent)
{ size = 0.618;
}

void Stones::clear()
{ foreach( Stone *s, stoneList )
    if ( s )
      delete s;
  stoneList.clear();
}
