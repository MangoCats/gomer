#ifndef GAMECONTROL_H
#define GAMECONTROL_H

class Board;
class Stone;
#include <QObject>
#include <QPointF>
#include "board.h"
#include "stone.h"

class GameControl : public QObject
{
    Q_OBJECT
public:
    explicit  GameControl( Board *pbp, QObject *parent = nullptr);
        void  start();
       qreal  stoneSize();
        void  proposeMove( int, int );

signals:
        void  stateMessage( QString );
        void  newStonePlaced( Stone * );

public slots:
        void  clickAt( QPointF );
        void  hover( QPointF );

public:
  QPointer<Board> bp;
};

#endif // GAMECONTROL_H
