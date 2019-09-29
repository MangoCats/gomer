#ifndef STONE_H
#define STONE_H

class Board;
#include <QObject>
#include <QPointer>
#include "board.h"

class Stone : public QObject
{ Q_OBJECT
public:
    explicit Stone( int px, int py, int pc, Board *pb, QObject *parent = nullptr);
            ~Stone() {}
signals:

public slots:

public:
             int  x,y,c;
  QPointer<Board> board;
};

#endif // STONE_H
