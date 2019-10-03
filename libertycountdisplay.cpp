#include "libertycountdisplay.h"

LibertyCountDisplay::LibertyCountDisplay( BoardScene *bsp, QObject *parent ) : QObject( parent )
{ scene = bsp;
  show = false;
}

void LibertyCountDisplay::clearCounts()
{ if ( scene == nullptr )
    return;
  foreach( QGraphicsSimpleTextItem *tp, counts )
    { tp->setVisible( false );
      scene->removeMyItem( tp );
      delete( tp );
    }
  counts.clear();
}

void LibertyCountDisplay::updateCounts()
{ clearCounts();
  if ( !show ) return;
  if ( scene == nullptr ) return;
  Board *bp = scene->bp;
  if ( bp == nullptr ) return;
  Stones *stp = bp->stones;
  if ( stp == nullptr ) return;
  foreach( StoneGroup *gp, stp->groupList )
    { int lc = gp->libertyCount();
      QBrush cb;
      if       ( lc < 2 ) cb = QBrush( QColor( 255,  0,  0 ) );
       else if ( lc < 3 ) cb = QBrush( QColor( 255,128,  0 ) );
       else if ( lc < 4 ) cb = QBrush( QColor( 128,192,  0 ) );
       else if ( lc < 8 ) cb = QBrush( QColor(   0,255,  0 ) );
       else               cb = QBrush( QColor(   0,255,255 ) );
      QFont cf = QFont( "Ubuntu" );
      cf.setPointSizeF( 0.6 );
      foreach( Stone *s, gp->group )
        { QGraphicsSimpleTextItem *tp = new QGraphicsSimpleTextItem( s->ei );
          tp->setBrush( cb );
          tp->setFont( cf );
          if ( lc > 9 )
            tp->setScale( 0.6 );
          qreal xo = (lc > 9) ? -0.4 : -0.3;
          qreal yo = (lc > 9) ? -0.4 : -0.65;
          tp->setPos( (qreal)s->x + xo, (qreal)s->y + yo );
          tp->setText( QString::number(lc) );
          qDebug( "adding lc" );
          // sp->addMyItem( tp );
          counts.append( tp );
        }
    }
}
