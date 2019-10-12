#ifndef TERRITORYGROUP_H
#define TERRITORYGROUP_H

class TerritorySquare;
#include <QObject>
#include "territorysquare.h"

class TerritoryGroup : public QObject
{
    Q_OBJECT
public:
    explicit TerritoryGroup(QObject *parent = nullptr);

signals:

public slots:

public:
  QList<QPointer<TerritorySquare> > group;
                               int  c;
};

#endif // TERRITORYGROUP_H
