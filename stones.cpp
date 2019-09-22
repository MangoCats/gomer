#include "stones.h"

Stones::Stones(QObject *parent) : QObject(parent)
{

}

void Stones::clear()
{ foreach( Stone *s, stoneList )
    if ( s )
      delete s;
  stoneList.clear();
}
