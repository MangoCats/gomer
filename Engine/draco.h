#ifndef DRACO_H
#define DRACO_H

class Wyrm;
#include <QObject>
#include <QPointer>
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
    explicit Draco(QObject *parent = nullptr);

signals:

public slots:

public:
  QList<QPointer<Wyrm> > wpl;
};

#endif // DRACO_H
