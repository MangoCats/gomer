#ifndef BOARD_H
#define BOARD_H

class Stone;
class Stones;
#include <QObject>
#include <QPointer>
#include "stone.h"
#include "stones.h"

class Board : public QObject
{
    Q_OBJECT
public:
    explicit  Board(QObject *parent = nullptr);
             ~Board();
       qreal  stoneSize();
         int  stoneIndexAt( int, int );
       Stone *stoneAt( int, int );
        bool  placeNextStone( int, int );
        bool  isOnBoard( Stone * );
        bool  isOnBoard( int, int );
        bool  legalMove( int, int );

signals:

public slots:

public:
                       int  Xsize,Ysize;
           QPointer<Stones> stones;
  QVector<QPointer<Stone> > board;
};

#endif // BOARD_H
