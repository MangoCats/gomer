#ifndef STONES_H
#define STONES_H

class Stone;
class Board;
#include <QObject>
#include <QPointer>
#include "stone.h"
#include "board.h"

class Stones : public QObject
{ Q_OBJECT
public:
    explicit Stones( Board *parent = nullptr);
            ~Stones() { clear(); }
       void  clear();
        int  stoneAt( int, int );
       bool  placeNextStone( int, int );

signals:

public slots:

public:
   QPointer<Board> bp;
    QList<Stone *> stoneList;
            qreal  size;
};

#endif // STONES_H
