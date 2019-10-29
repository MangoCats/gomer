#ifndef SHIKO_H
#define SHIKO_H

class Chiiki;
class Game;
class Goban;
class Jiyu;
class Wyrm;
#include <QObject>
#include "chiiki.h"
#include "game.h"
#include "goban.h"
#include "jiyu.h"
#include "wyrm.h"

/**
 * @brief The Shiko 思考 class - thinking about the board position and game
 */
class Shiko : public QObject
{
    Q_OBJECT
public:
      explicit  Shiko( Game *p );
                Shiko( Shiko *tp, Game *p );
          void  clearGoban();
          void  resizeGoban();
  QList<qint32> allLegalMoves( qint32 c );
          bool  legalMove( qint32 x, qint32 y, qint32 c, bool anyColor = false );
          bool  legalMoveIndex( qint32 i, qint32 c, bool anyColor = false );
          bool  isSelfCapture( qint32 x, qint32 y, qint32 c );
          bool  selfCaptureRelief( qint32 x, qint32 y, qint32 c );
          bool  isKo( qint32 x, qint32 y, qint32 c );
          void  koEvalCapture( qint32 x, qint32 y, qint32 c, QString *state );
          void  goishiPlacedOnGoban( Goishi *ip );
        qint32  testLibertyCount( qint32 i, qint32 c );
        qint32  armLibertyCount( qint32 x, qint32 y, qint32 c );
          Wyrm *wyrmAt( qint32 x, qint32 y );
        qint32  collectWyrm( QList<Wyrm *> *fwplp, QList<Wyrm *> *owplp, qint32 x, qint32 y, qint32 c );
          void  mergeWyrms( Wyrm *wp, Wyrm *wp2 );
          void  wyrmCaptured( Wyrm *wp );
          void  addCaptureLiberties( Wyrm *wp );
          void  addCaptureLiberties( Goishi *ip );
          void  addCaptureLiberty( qint32 x, qint32 y, qint32 i, qint32 c );
       QString  showWyrms();
 QList<Chiho *> bensonsChiho( Goban *bbp, qint32 c );
          void  onlyWyrmsColored( Goban *bbp, qint32 c );
  QList<Wyrm *> allWyrms( Goban *bbp );
          void  clearWyrm( Goban *bbp, Wyrm *wp );
          bool  isVital( Wyrm *wp, Chiho *rp );
        qint32  vitalCount( Wyrm *wp, const QList<Chiho *>& crpl );
  QList<qint32> passEyes( Wyrm *wp, const QList<Chiho *>& chpl );
          void  evaluateLife();

signals:

public slots:

public:
        QPointer<Chiiki> cp;
          QPointer<Game> gp;
         QPointer<Goban> bp;
          QPointer<Jiyu> jp;
  QList<QPointer<Wyrm> > wpl;
            QStringList  stateHistory; // Previous board positions
};

#endif // SHIKO_H
