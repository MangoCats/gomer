#ifndef BOARD_H
#define BOARD_H

class Stone;
class Stones;
class Territory;
#include <QObject>
#include <QPointer>
#include "stone.h"
#include "stones.h"
#include "territory.h"

class Board : public QObject
{
    Q_OBJECT
public:
    explicit  Board(QObject *parent = nullptr);
             ~Board();
       qreal  stoneSize();
         int  stoneIndexAt( int, int );
       Stone *stoneAt( int, int );
       Stone *placeNextStone( int, int );
        bool  isOnBoard( Stone * );
        bool  isOnBoard( int, int );
        bool  legalMove( int, int );
         int  executeCaptures( int x, int y, int c );
        bool  selfCapture( int x, int y, int c );
         int  friendlyLibertyCount( int x, int y, int c );
         int  colorToMove();

signals:
        void  captured( Stone * );

public slots:

public:
                       int  Xsize,Ysize;
           QPointer<Stones> stones;
        QPointer<Territory> territory;
  QVector<QPointer<Stone> > board;
};

#endif // BOARD_H
