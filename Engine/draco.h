#ifndef DRACO_H
#define DRACO_H

class Goban;
class Shiko;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "shiko.h"
#include "wyrm.h"

/**
 * @brief The Draco class - a loosely connected group of
 *   Wyrms of the same color - connected more strongly by
 *   their player than any threats of disconnection by an
 *   opposing player.
 */
class Draco : public QObject
{
    Q_OBJECT
public:
                     explicit  Draco( Wyrm *wp, QList<Wyrm *> awpl, Shiko *p );
                         void  addConnections( QList<Wyrm *> awpl );
                         bool  addSolidConnections( QList<Wyrm *> awpl );
                         bool  addSingleConnection( QList<Wyrm *> awpl );
                         bool  addDoubleConnection( QList<Wyrm *> awpl );
                       qint32  singleCutCount( Wyrm *wp, Wyrm *awp );
  QList<QPair<qint32,qint32> > doubleCutPairs( Wyrm *wp, Wyrm *awp );
                       qint32  color();
                      QString  show();

signals:

public slots:

public:
              QPointer<Goban> bp;
              QPointer<Shiko> tp;
                QList<qint32> cutPoints; // Single point non-redundant cuts
QList<QPair<qint32, qint32> > cutPairs;  // Two point connections not otherwise made by single point cuts
       QList<QPointer<Wyrm> > wpl;       // Wyrms in this Draco
};

#endif // DRACO_H
