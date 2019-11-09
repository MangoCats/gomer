#ifndef RUIKEI_H
#define RUIKEI_H

class Goban;
class Kigo;
class Kogai;
class Shiko;
class Menseki;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "kigo.h"
#include "kogai.h"
#include "shiko.h"
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
    explicit  Ruikei( qint32 xs, qint32 ys, Shiko *p = nullptr );
              Ruikei( QDataStream &ds, Shiko *p = nullptr );
        void  toDataStream( QDataStream &ds ) const;
        bool  isValid();
        bool  matchOne( Wyrm *wp, Goban *bp );
        bool  matchBoth( Wyrm *wp1, Wyrm *wp2, Goban *bp );
      qint32  nEdges() const;
        bool  x0Edge() const;
        bool  y0Edge() const;
        bool  xSizeEdge() const;
        bool  ySizeEdge() const;

public:
   QVector<Kigo> kl;
 QPointer<Kogai> op; // Synopsis/outline of what is known about this situation
           bool  nGobanEdge; // North border is Ysize in orientation 0
           bool  eGobanEdge; // East  border is Xsize in orientation 0
           bool  sGobanEdge; // South border is Y -1  in orientation 0
           bool  wGobanEdge; // West  border is X -1  in orientation 0
};

#endif // RUIKEI_H
