#ifndef STONE_H
#define STONE_H

class Board;
#include <QObject>
#include <QPointer>
#include <QPoint>
#include "board.h"

class Stone : public QObject
{ Q_OBJECT
public:
    explicit  Stone( int px, int py, int pc, int pi, Board *pb, QObject *parent = nullptr);
             ~Stone() {}
       Stone *northNeighbor();
       Stone *southNeighbor();
       Stone *eastNeighbor();
       Stone *westNeighbor();
         int  libertyCount();
QList<QPoint> libertyList();


signals:

public slots:

public:
             int  x,y;   /// 0 indexed position coordinates
             int  c;     /// Color: 0 black, 1 white
             int  i;     /// index number in the Stones list
             int  g;     /// index number of group in the group list
  QPointer<Board> board;
};

#endif // STONE_H
