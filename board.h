#ifndef BOARD_H
#define BOARD_H

class Stones;
#include <QObject>
#include <QPointer>
#include "stones.h"

class Board : public QObject
{
    Q_OBJECT
public:
    explicit Board(QObject *parent = nullptr);
            ~Board();
signals:

public slots:

public:
              int  Xsize,Ysize;
  QPointer<Stones> stones;
};

#endif // BOARD_H
