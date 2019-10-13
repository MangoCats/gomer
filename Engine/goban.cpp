#include "goban.h"

Goban::Goban(Game *parent,qint32 xs,qint32 ys) : QObject(parent), gp(parent), Xsize(xs), Ysize(ys)
{ qDebug( "Goban constructor %d x %d", Xsize, Ysize );
  for ( int i = 0 ; i < nPoints() ; i++ )
    grid.append( nullptr );
  goishiChar = ".XO3456789";
}

/**
 * @brief Goban::state
 * @return a compact string representing all grid points and their occupying Goishi
 */
QString Goban::state()
{ QString s;
  for ( int i = 0 ; i < nPoints() ; i++ )
    { if ( grid.at(i) == nullptr )
        s.append( goishiChar.at(0) );
       else
        { qint32 c = grid.at(i)->color;
          if (( c > (goishiChar.size() - 2) ) || ( c < 0 ))
            s.append( "!" );
           else
            s.append( goishiChar.at(c+1) );
        }
    }
  return s;
}
