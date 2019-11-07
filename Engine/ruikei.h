#ifndef RUIKEI_H
#define RUIKEI_H

class Goban;
class Menseki;
class Wyrm;
#include <QObject>
#include "goban.h"
#include "menseki.h"
#include "wyrm.h"

/**
 * @brief The Ruikei 類型 class - a pattern for matching with pre-calculated
 *   analysis of the situation depicted.
 */
class Ruikei : public Menseki
{
    Q_OBJECT
public:
    explicit  Ruikei(QObject *parent = nullptr);
        bool  matchOne( Wyrm *wp, Goban *bp );
        bool  matchBoth( Wyrm *wp1, Wyrm *wp2, Goban *bp );
        bool  indexToXY( qint32 i, qint32 *x, qint32 *y );
      qint32  xyToIndex( qint32 x, qint32 y );
      qint32  nPoints() { return rows * columns; }
      qint32  Xsize();
      qint32  Ysize();
        bool  testTransforms();

signals:

public slots:

public:
  QByteArray  pattern;
      qint32  rows,columns,orientation;
};

#endif // RUIKEI_H
