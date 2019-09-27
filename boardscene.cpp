#include "boardscene.h"

BoardScene::BoardScene( Board *pbp, QObject *parent ) : QGraphicsScene ( parent )
{ bp = pbp;
  backBrush  = QBrush( QColor( 128,128, 64 ) );
  lineBrush  = QBrush( QColor(   0,  0,  0 ) );
  blackBrush = QBrush( QColor(   0,  0,  0 ) );
  whiteBrush = QBrush( QColor( 255,255,255 ) );
}
