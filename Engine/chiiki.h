#ifndef CHIIKI_H
#define CHIIKI_H

class Goban;
class Ryoiki;
class Shiko;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "ryoiki.h"
#include "shiko.h"
#include "wyrm.h"

/**
 * @brief The Chiiki 地域 class - calculating and estimating territory
 *   the collection of Ryoiki for the current Goban position
 */
class Chiiki : public QObject
{
    Q_OBJECT
public:
    explicit  Chiiki( Shiko *p );
              Chiiki( Chiiki *pcp,Shiko *p );
        void  resizeGoban();
        void  clear();
        void  update();
        bool  ryoikiOwner( qint32 x, qint32 y, qint32 *rc );
        void  collectWyrms( qint32 x, qint32 y, Ryoiki *rp );
      qint32  colorAt( qint32 i );
     QString  showRyoiki();
     QString  showRyoiki( qint32 c );

signals:

public slots:

public:
                     QPointer<Goban> bp;
                     QPointer<Shiko> tp;
QVector<QVector<QPointer<Ryoiki> > > rGrid; // pointer back from every Goban intersection to the applicable Ryoiki, one for each color and one for all
  QVector<QList<QPointer<Ryoiki> > > rpm;   // the Ryoiki in list form, one list for each color and one for all Goishi
};

#endif // CHIIKI_H
