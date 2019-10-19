#ifndef SHIKO_H
#define SHIKO_H

class Game;
class Goban;
class Wyrm;
#include <QObject>
#include "game.h"
#include "goban.h"
#include "wyrm.h"

/**
 * @brief The Shiko 思考 class - thinking about the board position and game
 */
class Shiko : public QObject
{
    Q_OBJECT
public:
    explicit  Shiko(Goban *pbp, Game *parent);
              Shiko(Shiko *tp, Game *parent);
        void  clearGoban();
        bool  legalMove( qint32 x, qint32 y, qint32 c );
        bool  isSelfCapture( qint32 x, qint32 y, qint32 c );
        bool  selfCaptureRelief( qint32 x, qint32 y, qint32 c );
        void  goishiPlacedOnGoban( Goishi *ip );
        Wyrm *wyrmAt( qint32 x, qint32 y );
      qint32  collectWyrm( QList<Wyrm *> *fwplp, QList<Wyrm *> *owplp, qint32 x, qint32 y, qint32 c );
        void  mergeWyrms( Wyrm *wp, Wyrm *wp2 );
        void  wyrmCaptured( Wyrm *wp );
        void  addCaptureLiberties( Wyrm *wp );
        void  addCaptureLiberties( Goishi *ip );
        void  addCaptureLiberty( qint32 x, qint32 y, qint32 i, qint32 c );
     QString  showWyrms();

signals:

public slots:

public:
          QPointer<Game> gp;
         QPointer<Goban> bp;
  QList<QPointer<Wyrm> > wpl;
};

#endif // SHIKO_H
