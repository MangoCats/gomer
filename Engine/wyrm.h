#ifndef WYRM_H
#define WYRM_H

class Chiho;
class Goishi;
class Ryoiki;
class Shiko;
#include <QObject>
#include "chiho.h"
#include "goishi.h"
#include "ryoiki.h"
#include "shiko.h"

#define WYRM_LIVE       1
#define WYRM_UNSETTLED  0
#define WYRM_DEAD      -1

/**
 * @brief The Wyrm class - A solidly connected group of Goishi
 *   aka Worm, Chain, String, Block, Group, Army.
 */
class Wyrm : public Chiho
{
    Q_OBJECT
public:
      explicit  Wyrm( Goishi *ip, Shiko *p );
                Wyrm(   Wyrm *wp, Shiko *p );
          void  addGoishi( Goishi *ip );
          void  mergeLibertyList( QList<qint32> );
  QList<qint32> getLibertyList( Goishi *ip );
          void  addWyrm( Wyrm *wp );
        qint32  color();
          bool  removeLiberty( qint32 );
          bool  addLiberty( qint32 );
          bool  inAtari() { return ( libertyList.size() == 1 ); }
       QString  show();
QList<Ryoiki *> adjacentRyoiki();
          void  addRyoiki( Chiiki *cp, qint32 x, qint32 y, QList<Ryoiki *>& arplp );

signals:

public slots:

public:
  QList<QPointer<Goishi> > ipl;          // List of all Goishi in this Wyrm
             QList<qint32> libertyList;  // List of all this Wyrm's liberties, by grid index
                   qint32  lifeOrDeath;  // Current calculated status
           QPointer<Shiko> tp;           // The Shiko which holds this Wyrm

};

#endif // WYRM_H
