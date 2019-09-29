#ifndef BOARDRECT_H
#define BOARDRECT_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "boardrectobject.h"

class BoardRect : public QGraphicsRectItem
{
public:
           BoardRect( const QRectF &rect, QGraphicsItem *parent = nullptr );
          ~BoardRect() {}

protected:
     void  mousePressEvent(QGraphicsSceneMouseEvent *event);
     void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public:
  BoardRectObject  op;
};

#endif // BOARDRECT_H
