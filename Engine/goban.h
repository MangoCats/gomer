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
 */
class Goban : public QObject
{
    Q_OBJECT
public:
    explicit  Goban(Game *parent = nullptr, qint32 xs=19, qint32 ys=19);
      qint32  nPoints() { return Xsize * Ysize; }
     QString  state();
      Goishi *takeGoishi( qint32 i );
        bool  placeGoishi( Goishi *ip, qint32 i );
        bool  placeGoishiAt( Goishi *ip, qint32 x, qint32 y );
        bool  placeGoishiAt( Goishi *ip, QString pos );
        bool  onBoard( qint32 x, qint32 y );

signals:

public slots:

public:
              QPointer<Game> gp;
                     qint32  Xsize,Ysize;
  QVector<QPointer<Goishi> > grid;
                    QString  goishiChar;
                QStringList  Xlabels,Ylabels;
};

#endif // GOBAN_H
