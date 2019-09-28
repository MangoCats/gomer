#include "boardscene.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>

BoardScene::BoardScene( Board *pbp, QObject *parent ) : QGraphicsScene ( parent )
{ bp = pbp;
  backBrush  = QBrush( QColor( 224,152, 64 ) );
  blackBrush = QBrush( QColor(   0,  0,  0 ) );
  whiteBrush = QBrush( QColor( 255,255,255 ) );
  linePen    = QPen  ( QColor(   0,  0,  0 ) );
  linePen.setWidth( 0 );
  drawGrid();
}

void BoardScene::drawGrid()
{ QGraphicsLineItem *lip;
  BoardRect *rip;
  setBackgroundBrush( blackBrush );
  rip = new BoardRect( QRectF( -2.0, -2.0, (qreal)(bp->Xsize +3), (qreal)(bp->Ysize + 3) ), nullptr );
  rip->setBrush( backBrush );
  rip->setPen( linePen );
  addItem( rip );

  for ( int x = 0; x < bp->Xsize; x++ )
    { lip = addLine( QLineF( (qreal)x, 0.0, (qreal)x, (qreal)(bp->Ysize - 1) ), linePen );
      lip->setAcceptedMouseButtons( Qt::NoButton );
    }
  for ( int y = 0; y < bp->Ysize; y++ )
    { lip = addLine( QLineF( 0.0, (qreal)y, (qreal)(bp->Xsize - 1), (qreal)y ), linePen );
      lip->setAcceptedMouseButtons( Qt::NoButton );
    }
}
