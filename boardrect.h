#ifndef BOARDRECT_H
#define BOARDRECT_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>


class BoardRect : public QGraphicsRectItem
{
public:
          BoardRect( const QRectF &rect, QGraphicsItem *parent = nullptr );
         ~BoardRect() {}

protected:
    void  mousePressEvent(QGraphicsSceneMouseEvent *event);
    void  mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // BOARDRECT_H
