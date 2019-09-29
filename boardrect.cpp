#include "boardrect.h"

BoardRect::BoardRect( const QRectF &rect, QGraphicsItem *parent ) : QGraphicsRectItem( rect, parent )
{}

void  BoardRect::mousePressEvent(QGraphicsSceneMouseEvent *event)
{ // QString msg = QString("press %1 %2").arg(event->buttonDownScenePos(event->button()).rx())
  //                                     .arg(event->buttonDownScenePos(event->button()).ry());
  // qDebug( qPrintable( msg ) );
  QGraphicsRectItem::mousePressEvent(event);
  event->accept();
}

void  BoardRect::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{ op.clickAt( event->buttonDownScenePos(event->button()) );
  // QString msg = QString("release %1 %2").arg(event->buttonDownScenePos(event->button()).rx())
  //                                       .arg(event->buttonDownScenePos(event->button()).ry());
  // qDebug( qPrintable( msg ) );
  QGraphicsRectItem::mousePressEvent(event);
  event->accept();
}

