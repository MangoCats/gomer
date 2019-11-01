#ifndef GOBAN_H
#define GOBAN_H

class Chiho;
class Game;
class Goishi;
#include <QObject>
#include <QPointer>
#include <QVector>
#include "chiho.h"
#include "game.h"
#include "goishi.h"

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
class Goban : public QObject
{
    Q_OBJECT
public:
     explicit  Goban( Game *p, qint32 xs=19, qint32 ys=19 );
               Goban( Goban *bp, Game *p );
         bool  resize( qint32 xs, qint32 ys );
       qint32  nPoints() { return Xsize * Ysize; }
      QString  state();
       Goishi *takeGoishi( qint32 i );
         void  removeGoishi( Goishi *ip );
         bool  placeGoishi( Goishi *ip, qint32 i );
         bool  placeGoishiAt( Goishi *ip, qint32 x, qint32 y );
         bool  placeGoishiAt( Goishi *ip, QString v );
         bool  vertexToXY( QString v, qint32 *x, qint32 *y );
         bool  indexNeighbors( qint32 i, qint32 j );
       qint32  vertexToIndex( QString v );
      QString  indexToVertex( qint32 i );
      QString  xyToVertex( qint32 x, qint32 y );
         bool  indexToXY( qint32 i, qint32 *x, qint32 *y );
       qint32  xyToIndex( qint32 x, qint32 y ) { return x + (Xsize * y); }
        QChar  colorToChar( qint32 c );
       Goishi *goishiAt( qint32 x, qint32 y );
       Goishi *goishiAt( QString v );
       Goishi *goishi( qint32 i );
       qint32  color( qint32 i );
         bool  onBoard( qint32 x, qint32 y );
         bool  onBoard( QString v );
      QString  showBoard();
      QString  showChiho( Chiho *hp );
      QString  xAxisLabels();
      QString  centerString( QString s, qint32 len );
      QString  asciiGoishi( qint32 x, qint32 y );
QList<Chiho *> fillByRule( qint32 c, bool rule );
         bool  fill( qint32 x, qint32 y, qint32 c, bool rule, Chiho *hp );
         bool  fillRuleCheck( qint32 x, qint32 y, qint32 c, bool rule, Chiho *hp );

signals:
         void  boardChanged( QString );

public slots:

public:
              QPointer<Game> gp;
                     qint32  Xsize,Ysize;
  QVector<QPointer<Goishi> > grid;
               QList<qint32> Xdots,Ydots;
                    QString  goishiChar;
                QStringList  Xlabels,Ylabels;
};

#endif // GOBAN_H
