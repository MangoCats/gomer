#ifndef BOARDRECT_H
#define BOARDRECT_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>


class BoardRect : public QGraphicsRectItem
{
public:
     BoardRect( const QRectF &rect, QGraphicsItem *parent = nullptr );
    ~BoardRect() {}
};

#endif // BOARDRECT_H
