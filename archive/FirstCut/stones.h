#ifndef STONES_H
#define STONES_H

class Stone;
class Board;
class StoneGroup;
#include <QObject>
#include <QPointer>
#include "stone.h"
#include "board.h"
#include "stonegroup.h"

class Stones : public QObject
{ Q_OBJECT
public:
     explicit Stones( Board *parent = nullptr);
             ~Stones() { clear(); }
        void  clear();
         int  stoneAt( int, int );
       Stone *placeNextStone( int, int );
        void  computeGroups();
        void  clearGroups();
        bool  isInAGroup( Stone * );
         int  isInGroup( Stone * );
QList<QPoint> atariList( int color );
         int  colorToMove() { return (stoneList.size() & 1); } // Black is 0, White is 1

signals:

public slots:

public:
             QPointer<Board> bp;
     QList<QPointer<Stone> > stoneList;
QList<QPointer<StoneGroup> > groupList;
                      qreal  size; // For drawing
};

#endif // STONES_H
