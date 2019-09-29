#include "stonegroup.h"

StoneGroup::StoneGroup( Stone *fsp, QObject *parent) : QObject(parent)
{ group.append( fsp );
  c = fsp->c;
}

bool StoneGroup::isInGroup( Stone *sp )
{ foreach( Stone *gp, group )
    if ( sp->i == gp->i )
      return true;
  return false;
}

void StoneGroup::addStone( Stone *sp )
{ if ( sp->c != c )
    { qDebug( "StoneGroup::addStone color mismatch, not adding stone" );
      return;
    }
  group.append( sp );
}

/**
 * @brief StoneGroup::liberties
 * @return total number of liberties held by the group
 */
int StoneGroup::liberties()
{ int lc = 0;
  foreach( Stone *sp, group )
    lc += sp->liberties();
  return lc;
}
