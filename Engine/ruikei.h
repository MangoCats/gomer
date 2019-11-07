#ifndef RUIKEI_H
#define RUIKEI_H

class Goban;
class Kigo;
class Menseki;
class Wyrm;
#include <QObject>
#include "goban.h"
#include "kigo.h"
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
    explicit  Ruikei( qint32 xs, qint32 ys, QObject *parent = nullptr);
        bool  matchOne( Wyrm *wp, Goban *bp );
        bool  matchBoth( Wyrm *wp1, Wyrm *wp2, Goban *bp );
  QByteArray  kigoList();
        bool  fromByteArray( QByteArray );

public:
  QVector<Kigo> kl;
};

#endif // RUIKEI_H
