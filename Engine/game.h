#ifndef GAME_H
#define GAME_H

class Goban;
class Gosu;
class Player;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "gosu.h"
#include "player.h"

/**
 * @brief The Game class - encapsulates a single game play
 */
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(qint32 nPlayers = 2, qint32 xs = 19, qint32 ys = 19, QObject *parent = nullptr);

signals:

public slots:

public:
           QPointer<Goban> bp;
    QList<QPointer<Gosu> > spl;
  QList<QPointer<Player> > ppl;
                    qreal  komi;
                   qint32  np;
};

#endif // GAME_H
