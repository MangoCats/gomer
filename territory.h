#ifndef TERRITORY_H
#define TERRITORY_H

class Board;
class BoardScene;
class TerritoryGroup;
class TerritorySquare;
#include <QObject>
#include "board.h"
#include "boardscene.h"
#include "territorygroup.h"
#include "territorysquare.h"

class Territory : public QObject
{
    Q_OBJECT
public:
    explicit Territory( Board *parent = nullptr );
        void clear();
        void update();
        void clearDisplay();
        void updateDisplay();
        void tFill( int, int, int );
        void tCheck( int, int, int );
        bool territoryColor( int, int, int * );

signals:

public slots:
        void  setShow( bool s ) { show = s; updateDisplay(); }
        void  setScene( BoardScene *scp );

public:
                  QPointer<Board> bp;
             QPointer<BoardScene> scene;
QList<QPointer<TerritorySquare> > squareList;
 QList<QPointer<TerritoryGroup> > groupList;
                     QVector<int> gi; // Group index map

                           qreal  size; // For drawing
                            bool  show;
};

#endif // TERRITORY_H
