#ifndef BOARDSCENE_H
#define BOARDSCENE_H

class Board;
#include <QGraphicsScene>
#include "board.h"
#include "boardrect.h"

class BoardScene : public QGraphicsScene
{ Q_OBJECT
public:
            BoardScene( Board *pbp, QObject *parent = nullptr );
      void  drawGrid();

     Board *bp;
    QBrush  backBrush,blackBrush,whiteBrush;
      QPen  linePen;
};

#endif // BOARDSCENE_H
