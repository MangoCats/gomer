#ifndef MENSEKI_H
#define MENSEKI_H

#include <QObject>


/**
 * @brief The Menseki 面積 class - base class
 *   for play areas like Goban and Ruikei
 */
class Menseki : public QObject
{
    Q_OBJECT
public:
     explicit  Menseki( qint32 xs, qint32 ys, QObject *parent = nullptr );
               Menseki( QObject *parent = nullptr );
         void  init();
        QChar  colorToChar( qint32 c );
         bool  indexNeighbors( qint32 i, qint32 j );
      QString  indexToVertex( qint32 i );
         bool  indexToXY( qint32 i, qint32 *x, qint32 *y );
       qint32  vertexToIndex( QString v );
         bool  vertexToXY( QString v, qint32 *x, qint32 *y );
       qint32  xyToIndex( qint32 x, qint32 y );
      QString  xyToVertex( qint32 x, qint32 y );
       qint32  nPoints() { return rows * columns; }
       qint32  Xsize();
       qint32  Ysize();
         bool  testTransforms();

signals:

public slots:

public:
      QString  goishiChar;
  QStringList  Xlabels,Ylabels;
       qint32  rows,columns,orientation;
};

#endif // MENSEKI_H
