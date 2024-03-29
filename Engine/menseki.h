#ifndef MENSEKI_H
#define MENSEKI_H

class Dotei;
#include <QObject>
#include <QPointer>
#include "dotei.h"

/**
 * @brief The Menseki 面積 class - base class
 *   for play areas like Goban and Ruikei
 */
class Menseki : public QObject
{
    Q_OBJECT
public:
     explicit  Menseki( qint32 xs, qint32 ys, Dotei *p = nullptr );
               Menseki( Dotei *p = nullptr );
         void  init();
        QChar  colorToChar( qint32 c ) const;
         bool  indexNeighbors( qint32 i, qint32 j ) const;
      QString  indexToVertex( qint32 i ) const;
         bool  indexToXY( qint32 i, qint32 *x, qint32 *y ) const;
       qint32  vertexToIndex( QString v ) const;
         bool  vertexToXY( QString v, qint32 *x, qint32 *y ) const;
       qint32  xyToIndex( qint32 x, qint32 y ) const;
      QString  xyToVertex( qint32 x, qint32 y ) const;
       qint32  nPoints() const { return rows * columns; }
       qint32  Xsize() const;
       qint32  Ysize() const;
         bool  testTransforms();

  QPointer<Dotei> vp;
          qint32  rows,columns,orientation;
};

#endif // MENSEKI_H
