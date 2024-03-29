#ifndef GOBAN_H
#define GOBAN_H

class Chiho;
class Goishi;
class Menseki;
class Shiai;
#include <QObject>
#include <QPointer>
#include <QVector>
#include "chiho.h"
#include "goishi.h"
#include "menseki.h"
#include "shiai.h"

// Notional no player and undetermined player indices (like colors, but not colors)
#define NO_PLAYER -1
#define UNDETERMINED_PLAYER -2

// X coordinates when in Gosu, Y coordinate is the color of the Gosu owner
#define GOSU_BOWL -2
#define GOSU_LID  -3

/**
 * @brief The Goban 碁盤 class - a generalized playing board
 *
 * grid   is a zero indexed integer pair x,y description of a board intersection point
 * index  is a single integer description of a board intersection point = x + Xsize * y
 * vertex is a string-label description of a board intersection point
 */
class Goban : public Menseki
{
    Q_OBJECT
public:
     explicit  Goban( Shiai *p, qint32 xs=19, qint32 ys=19 );
               Goban( Goban *bp, Shiai *p );
         bool  resize( qint32 xs, qint32 ys );
      QString  state();
       Goishi *takeGoishi( qint32 i );
         void  removeGoishi( Goishi *ip );
         bool  placeGoishi( Goishi *ip, qint32 i );
         bool  placeGoishiAt( Goishi *ip, qint32 x, qint32 y );
         bool  placeGoishiAt( Goishi *ip, QString v );
       Goishi *goishiAt( qint32 x, qint32 y );
       Goishi *goishiAt( QString v );
       Goishi *goishi( qint32 i );
       qint32  color( qint32 i );
         bool  onBoard( qint32 x, qint32 y );
         bool  onBoard( QString v );
         bool  onEdge( qint32 i ) const;
      QString  showBoard();
      QString  showChiho( Chiho *hp );
      QString  xAxisLabels();
      QString  centerString( QString s, qint32 len );
      QString  asciiGoishi( qint32 x, qint32 y );
QList<Chiho *> fillByRule( qint32 c, bool rule );
         bool  fill( qint32 x, qint32 y, qint32 c, bool rule, Chiho *hp );
         bool  fillRuleCheck( qint32 x, qint32 y, qint32 c, bool rule, Chiho *hp );
         bool  chihoListContains( QList<Chiho *> hpl, qint32 i );
         bool  chihoOnEdge( Chiho *hp ) const;
         void  chihoXYsize( Chiho *hp, qint32 &x, qint32 &y ) const;
         void  chihoXYlimits( Chiho *hp, qint32 &minX, qint32 &minY, qint32 &maxX, qint32 &maxY ) const;

signals:
         void  boardChanged( QString );

public slots:

public:
             QPointer<Shiai> gp;
  QVector<QPointer<Goishi> > grid;
               QList<qint32> Xdots,Ydots;
};

#endif // GOBAN_H
