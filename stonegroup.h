#ifndef STONEGROUP_H
#define STONEGROUP_H

class Stone;
#include <QObject>
#include "stone.h"

class StoneGroup : public QObject
{
    Q_OBJECT
public:
    explicit StoneGroup( Stone *fsp, QObject *parent = nullptr);
        bool isInGroup( Stone * );
        void addStone( Stone * );
         int size() { return group.size(); }
         int liberties();

signals:

public slots:

public:
  QList<QPointer<Stone> > group;
                     int  c;
};

#endif // STONEGROUP_H
