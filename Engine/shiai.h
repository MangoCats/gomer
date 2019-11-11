#ifndef SHIAI_H
#define SHIAI_H

class Dotei;
class Goban;
class Goishi;
class Gosu;
class Player;
class Sakudo;
class Shiko;
#include <QObject>
#include <QPointer>
#include "dotei.h"
#include "goban.h"
#include "goishi.h"
#include "gosu.h"
#include "player.h"
#include "sakudo.h"
#include "shiko.h"

/**
 * @brief The Shiai 試合 class - encapsulates a single game play
 */
class Shiai : public QObject
{
    Q_OBJECT
public:
    explicit Shiai( QStringList playerNames, qint32 xs = 19, qint32 ys = 19, QObject *p = nullptr );
             Shiai( Shiai *gp, QObject *p = nullptr );
        void fillGosu();
        void clearGoban();
        void clearGoishi( Goishi * );
        bool resizeGoban( qint32 xs, qint32 ys );
     QString showBoard();
        bool playGoishi( qint32 x, qint32 y, qint32 c );
        bool playGoishi( QString v, qint32 c );
        bool playGoishiIndex( qint32 i, qint32 c );
        void capture( Wyrm * );
        void pass();
        void advancePlayer();

signals:

public slots:

public:
           QPointer<Goban> bp;
    QList<QPointer<Gosu> > spl;
  QList<QPointer<Player> > ppl;
          QPointer<Sakudo> mp;
           QPointer<Shiko> tp;
                    Dotei  v;            // Vertex labels
                    qreal  komi;
                   qint32  np;           // Number of players
                   qint32  pt;           // Which player's turn is is?
};

#endif // GAME_H
