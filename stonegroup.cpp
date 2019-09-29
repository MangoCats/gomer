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
 * @brief StoneGroup::libertyCount
 * @return total number of liberties held by the group
 * TODO: fix inaccuracy, two stones may be counting the same liberty
 */
int StoneGroup::libertyCount()
{ QList<QPoint> ll = libertyList();
  return ll.size();
}

QList<QPoint> StoneGroup::libertyList()
{ QList<QPoint> ll;
  foreach( Stone *sp, group )
    { QList<QPoint> sl = sp->libertyList();
      foreach( QPoint lp, sl )
        if ( !ll.contains( lp ) )
          ll.append( lp );
    }
  return ll;
}
