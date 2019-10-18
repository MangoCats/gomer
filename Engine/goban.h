#ifndef GOBAN_H
#define GOBAN_H

class Game;
class Goishi;
#include <QObject>
#include <QPointer>
#include "game.h"
#include "goishi.h"

/**
 * @brief The Goban class - a generalized playing board
 *
 * grid   is a zero indexed integer pair x,y description of a board intersection point
 * index  is a single integer description of a board intersection point = x + Xsize * y
 * vertex is a string-label description of a board intersection point
 */
class Goban : public QObject
{
    Q_OBJECT
public:
    explicit  Goban(Game *parent = nullptr, qint32 xs=19, qint32 ys=19);
        bool  resize( qint32 xs, qint32 ys );
      qint32  nPoints() { return Xsize * Ysize; }
     QString  state();
      Goishi *takeGoishi( qint32 i );
        void  removeGoishi( Goishi *ip );
        bool  placeGoishi( Goishi *ip, qint32 i );
        bool  placeGoishiAt( Goishi *ip, qint32 x, qint32 y );
        bool  placeGoishiAt( Goishi *ip, QString pos );
        bool  vertexToXY( QString pos, qint32 *x, qint32 *y );
        bool  indexToXY( qint32 i, qint32 *x, qint32 *y );
      qint32  xyToIndex( qint32 x, qint32 y ) { return x + (Xsize * y); }
     QString  indexToVertex( qint32 i );
     QString  xyToVertex( qint32 x, qint32 y );
      Goishi *goishiAt( qint32 x, qint32 y );
        bool  onBoard( qint32 x, qint32 y );
     QString  showBoard();
     QString  xAxisLabels();
     QString  centerString( QString s, qint32 len );
     QString  asciiGoishi( qint32 x, qint32 y );


signals:
        void  boardChanged( QString );

public slots:

public:
              QPointer<Game> gp;
                     qint32  Xsize,Ysize;
               QList<qint32> Xdots,Ydots;
  QVector<QPointer<Goishi> > grid;
                    QString  goishiChar;
                QStringList  Xlabels,Ylabels;
};

#endif // GOBAN_H
