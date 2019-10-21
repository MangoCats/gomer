#ifndef SAKUDO_H
#define SAKUDO_H

class Game;
class Goban;
class Shiko;
#include <QObject>
#include <QPointer>
#include <QRandomGenerator>
#include "game.h"
#include "goban.h"
#include "shiko.h"

/**
 * @brief The Sakudo 策動 class - selects moves for computer players
 */
class Sakudo : public QObject
{
    Q_OBJECT
public:
    explicit  Sakudo( Game *p );
     QString  genmove( qint32 c );
     QString  genmoveRandy( qint32 c );
     QString  genmoveKilroy( qint32 c );
     QString  genmoveTerry( qint32 c );
        bool  canBeAttacked( Wyrm *wp, qint32 c );

signals:

public slots:

public:
  QRandomGenerator  rng;
     QPointer<Game> gp;
    QPointer<Goban> bp;
    QPointer<Shiko> tp;
            qint32  level; // Also, strategy
};

#endif // SAKUDO_H
