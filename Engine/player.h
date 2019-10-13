#ifndef PLAYER_H
#define PLAYER_H

class Goban;
class Gosu;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "gosu.h"

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(Gosu *parent = nullptr);

signals:

public slots:

public:
           QString  name;
    QPointer<Goban> bp;
    QPointer <Gosu> sp;
};

#endif // PLAYER_H
