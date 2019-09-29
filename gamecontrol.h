#ifndef GAMECONTROL_H
#define GAMECONTROL_H

class Board;
#include <QObject>
#include <QPointF>
#include "board.h"

class GameControl : public QObject
{
    Q_OBJECT
public:
    explicit  GameControl( Board *pbp, QObject *parent = nullptr);
        void  start();
       qreal  stoneSize();

signals:
        void  stateMessage( QString );

public slots:
        void  clickAt( QPointF );

public:
  QPointer<Board> bp;
};

#endif // GAMECONTROL_H
