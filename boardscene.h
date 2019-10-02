#ifndef BOARDSCENE_H
#define BOARDSCENE_H

class Board;
class BoardRect;
class BoardRectObject;
class Stone;
class LibertyCountDisplay;
#include <QPointer>
#include <QGraphicsScene>
#include "board.h"
#include "boardrect.h"
#include "boardrectobject.h"
#include "stone.h"
#include "libertycountdisplay.h"

class BoardScene : public QGraphicsScene
{ Q_OBJECT
public:
            BoardScene( Board *pbp, QObject *parent = nullptr );
      void  drawGrid();
      void  removeMyItem( QGraphicsItem *ip ) { removeItem(ip); }
      void  addMyItem( QGraphicsItem *ip ) { addItem(ip); }

public slots:
      void  placeNewStone(Stone *);
      void  stoneCaptured(Stone *);
      void  showLibertyCounts( bool s );

public:
              QPointer<Board> bp;
                   BoardRect *rp;
    QPointer<BoardRectObject> op;
QPointer<LibertyCountDisplay> lp;
                      QBrush  backBrush,blackBrush,whiteBrush;
                        QPen  linePen,blackPen,whitePen;
};

#endif // BOARDSCENE_H
