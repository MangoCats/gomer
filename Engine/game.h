#ifndef GAME_H
#define GAME_H

class Goban;
class Goishi;
class Gosu;
class GtpHandler;
class Player;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "goishi.h"
#include "gosu.h"
#include "gtphandler.h"
#include "player.h"

/**
 * @brief The Game class - encapsulates a single game play
 */
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QStringList playerNames, qint32 xs = 19, qint32 ys = 19, QObject *parent = nullptr);
        void clearBoard();
        void clearGoishi( Goishi * );

signals:

public slots:

public:
      QPointer<GtpHandler> hp;
           QPointer<Goban> bp;
    QList<QPointer<Gosu> > spl;
  QList<QPointer<Player> > ppl;
                    qreal  komi;
                   qint32  np;
};

#endif // GAME_H
