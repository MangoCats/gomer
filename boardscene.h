#ifndef BOARDSCENE_H
#define BOARDSCENE_H

class Board;
class BoardRect;
class BoardRectObject;
#include <QPointer>
#include <QGraphicsScene>
#include "board.h"
#include "boardrect.h"
#include "boardrectobject.h"

class BoardScene : public QGraphicsScene
{ Q_OBJECT
public:
            BoardScene( Board *pbp, QObject *parent = nullptr );
      void  drawGrid();

public slots:
      void  placeNewStone(int,int,int);

public:
          QPointer<Board> bp;
               BoardRect *rp;
QPointer<BoardRectObject> op;
                  QBrush  backBrush,blackBrush,whiteBrush;
                    QPen  linePen,blackPen,whitePen;
};

#endif // BOARDSCENE_H
