#ifndef SAKUDO_H
#define SAKUDO_H

class Chiiki;
class Game;
class Goban;
class Shiko;
#include <QObject>
#include <QPointer>
#include <QRandomGenerator>
#include "chiiki.h"
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
              Sakudo( Sakudo *mp, Game *p );
        bool  allInOwnRyoiki( qint32 c, const QList<qint32>& ml );
QList<qint32> removeOwnRyoiki( qint32 c, const QList<qint32>& ml );
QList<qint32> removePassEyes( const QList<qint32>& ml );
     QString  genmove( qint32 c );
     QString  firstMove( qint32 c );
     QString  genmoveRandy( qint32 c );
     QString  genmoveKilgore( qint32 c );
     QString  genmoveKilkenny( qint32 c );
     QString  genmoveEasyD( qint32 c );
     QString  genmoveTerry( qint32 c );
     QString  genmoveMonty( qint32 c );
        bool  playOneMonty( qint32 c, qint32 i );
        bool  finishRandomGame();
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
