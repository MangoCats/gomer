#ifndef CHIIKI_H
#define CHIIKI_H

class Goban;
class Shiko;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "shiko.h"
#include "wyrm.h"

/**
 * @brief The Ryoiki 領域 class - a Region, defined as a group
 *   of Goban intersection points - like a Wyrm of free space.
 */
class Ryoiki : public QObject
{
    Q_OBJECT
public:
    explicit  Ryoiki( qint32 plp, Chiiki *p );
              Ryoiki( Ryoiki *rp, Chiiki *p );
        void  addGobanIndex( qint32 i ) { if ( !bi.contains( i ) ) bi.append( i ); }
     QString  show();

                qint32  player; // Player's Goishi which bound this Ryoiki, if == total number of players then this Ryoiki borders any color Goishi
                qint32  owner;  // For Ryoiki which may border any Goishi, if only one color is bordered then this is the owner
        QPointer<Goban> bp;     // Goban this is on
          QList<qint32> bi;     // List of Goban index points in this Ryoiki
 QList<QPointer<Wyrm> > wpl;    // All Wyrm which border this Ryoiki
};

/**
 * @brief The Chiiki 地域 class - calculating and estimating territory
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
        bool  addIndex( qint32 );
        bool  fill( qint32 x, qint32 y, Ryoiki *rp );
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
