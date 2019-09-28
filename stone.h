#ifndef STONE_H
#define STONE_H

class Board;
#include <QObject>
#include <QPointer>
#include "board.h"

class Stone : public QObject
{ Q_OBJECT
public:
    explicit Stone(QObject *parent = nullptr);
            ~Stone() {}
signals:

public slots:

public:
             int  x,y;
  QPointer<Board> board;
};

#endif // STONE_H
