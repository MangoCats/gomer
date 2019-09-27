#ifndef BOARDSCENE_H
#define BOARDSCENE_H

class Board;
#include <QGraphicsScene>
#include "board.h"

class BoardScene : public QGraphicsScene
{
public:
            BoardScene( Board *pbp, QObject *parent = nullptr );

     Board *bp;
    QBrush  backBrush,lineBrush,blackBrush,whiteBrush;
};

#endif // BOARDSCENE_H
