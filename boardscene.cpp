#include "boardscene.h"

BoardScene::BoardScene( Board *pbp, QObject *parent ) : QGraphicsScene ( parent )
{ bp = pbp;
  backBrush  = QBrush( QColor( 128,128, 64 ) );
  blackBrush = QBrush( QColor(   0,  0,  0 ) );
  whiteBrush = QBrush( QColor( 255,255,255 ) );
  linePen    = QPen  ( QColor(   0,  0,  0 ) );
  linePen.setWidth( 0 );
  drawGrid();
}

void BoardScene::drawGrid()
{ for ( int x = 0; x < bp->Xsize; x++ )
    addLine( QLineF( (qreal)x, 0.0, (qreal)x, (qreal)(bp->Ysize - 1) ), linePen );
  for ( int y = 0; y < bp->Ysize; y++ )
    addLine( QLineF( 0.0, (qreal)y, (qreal)(bp->Xsize - 1), (qreal)y ), linePen );
}
