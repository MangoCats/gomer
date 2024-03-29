#include "boardscene.h"
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>

BoardScene::BoardScene( Board *pbp, QObject *parent ) : QGraphicsScene ( parent )
{ bp = pbp;
  rp = nullptr;
  lp = new LibertyCountDisplay( this );
  backBrush  = QBrush( QColor( 224,152, 64 ) );
  blackBrush = QBrush( QColor(   0,  0,  0 ) );
  whiteBrush = QBrush( QColor( 255,255,255 ) );
  blackPen   = QPen  ( QColor(  64, 64, 64 ) );
  blackPen.setWidth( 0 );
  whitePen   = QPen  ( QColor( 192,192,192 ) );
  whitePen.setWidth( 0 );
  linePen    = QPen  ( QColor(   0,  0,  0 ) );
  linePen .setWidth( 0 );
  drawGrid();
}

void BoardScene::drawGrid()
{ QGraphicsLineItem *lip;
  setBackgroundBrush( blackBrush );
  rp = new BoardRect( QRectF( -2.0, -2.0, (qreal)(bp->Xsize +3), (qreal)(bp->Ysize + 3) ), nullptr );
  rp->setBrush( backBrush );
  rp->setPen( linePen );
  op = &(rp->op);
  addItem( rp );

  for ( int x = 0; x < bp->Xsize; x++ )
    { lip = addLine( QLineF( (qreal)x, 0.0, (qreal)x, (qreal)(bp->Ysize - 1) ), linePen );
      lip->setAcceptedMouseButtons( Qt::NoButton );
    }
  for ( int y = 0; y < bp->Ysize; y++ )
    { lip = addLine( QLineF( 0.0, (qreal)y, (qreal)(bp->Xsize - 1), (qreal)y ), linePen );
      lip->setAcceptedMouseButtons( Qt::NoButton );
    }
}

void  BoardScene::showLibertyCounts( bool s )
{ if ( lp ) lp->setShow( s ); }

void  BoardScene::placeNewStone(Stone *sp)
{ if ( bp == nullptr )
    return;
  qreal ss = bp->stoneSize();
  qreal xc = ((qreal)sp->x) - ss*0.5;
  qreal yc = ((qreal)sp->y) - ss*0.5;
  if ( sp->c == 0 )
    sp->ei = addEllipse( xc,yc,ss,ss,blackPen,blackBrush );
   else
    sp->ei = addEllipse( xc,yc,ss,ss,whitePen,whiteBrush );
  if ( lp )
    lp->updateCounts();
  if ( bp->territory != nullptr )
    bp->territory->update();
}

void  BoardScene::stoneCaptured(Stone *sp)
{ if ( sp == nullptr )
    { qDebug( "ERROR: captured stone is null" );
      return;
    }
  if ( sp->ei == nullptr )
    { qDebug( "ERROR: captured stone has no graphics item" );
      return;
    }
  sp->ei->setVisible( false );
  removeItem( sp->ei );
  delete( sp->ei );
  sp->ei = nullptr;
}
