#ifndef GAME_H
#define GAME_H

class Goban;
class Goishi;
class Gosu;
class Player;
class Shiko;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "goishi.h"
#include "gosu.h"
#include "player.h"
#include "shiko.h"

/**
 * @brief The Game class - encapsulates a single game play
 */
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(QStringList playerNames, qint32 xs = 19, qint32 ys = 19, QObject *parent = nullptr);
             Game(Game *gp, QObject *parent = nullptr);
        void fillGosu();
        void clearGoban();
        void clearGoishi( Goishi * );
        bool resizeGoban( qint32 xs, qint32 ys );
     QString showBoard();
        bool playGoishi( qint32 x, qint32 y, qint32 c );
        void capture( Wyrm * );
        void pass();
        void advancePlayer();

signals:

public slots:

public:
           QPointer<Goban> bp;
    QList<QPointer<Gosu> > spl;
  QList<QPointer<Player> > ppl;
           QPointer<Shiko> tp;
                    qreal  komi;
                   qint32  np;           // Number of players
                   qint32  pt;           // Which player's turn is is?
};

#endif // GAME_H
