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
        bool  matchOne( Wyrm *wp, Goban *bp );
        bool  matchBoth( Wyrm *wp1, Wyrm *wp2, Goban *bp );
  QByteArray  kigoList();
        bool  fromByteArray( QByteArray );

public:
   QVector<Kigo> kl;
 QPointer<Kogai> op; // Synopsis/outline of what is known about this situation
};

#endif // RUIKEI_H
