#ifndef GOSU_H
#define GOSU_H

class Goban;
class Goishi;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "goishi.h"

/**
 * @brief The Gosu ゴス class - holds stones not on the board, either in the bowl or the lid.
 */
class Gosu : public QObject
{
    Q_OBJECT
public:
    explicit  Gosu( Goban *p );
              Gosu( Gosu *sp, Goban *p );
        void  empty();
        void  addGoishiToBowl( Goishi * );
        void  addGoishiToLid( Goishi * );
      qint32  goishiInBowl() { return bowl.size(); }
      qint32  goishiInLid()  { return  lid.size(); }
      Goishi *takeGoishiFromBowl();
      Goishi *takeGoishiFromLid();

signals:
        void  bowlCountChanged( qint32 );
        void  lidCountChanged( qint32 );

public:
           QPointer<Goban> bp;    /// Board this bowl is playing on
  QList<QPointer<Goishi> > bowl;  /// Stones yet to be played
  QList<QPointer<Goishi> > lid;   /// Stones captured
};

#endif // GOSU_H
