#ifndef STONES_H
#define STONES_H

class Stone;
#include <QObject>
#include <QPointer>
#include "stone.h"

class Stones : public QObject
{
    Q_OBJECT
public:
    explicit Stones(QObject *parent = nullptr);
            ~Stones() { clear(); }
       void  clear();

signals:

public slots:

public:
    QList<Stone *> stoneList;
};

#endif // STONES_H
