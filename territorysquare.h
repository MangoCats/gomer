#ifndef TERRITORYSQUARE_H
#define TERRITORYSQUARE_H

class Board;
#include <QObject>
#include <board.h>

class TerritorySquare : public QObject
{
    Q_OBJECT
public:
    explicit TerritorySquare(QObject *parent = nullptr);

signals:

public slots:

public:
                 int  x,y;   /// 0 indexed position coordinates
                 int  c;     /// Color: -1 undetermined, 0 black, 1 white
                 int  g;     /// index number of group in the group list
      QPointer<Board> board;
   QGraphicsRectItem *ri;    /// the square's image in the board scene
};

#endif // TERRITORYSQUARE_H
