#ifndef CHIIKI_H
#define CHIIKI_H

class Goban;
class Shiko;
class Ryoiki;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "shiko.h"

/**
 * @brief The Hata 旗 class - describes one grid point's territorial properties
 */
class Hata : public QObject
{
    Q_OBJECT
public:
      explicit  Hata( Chiiki *p, qint32 i, qint32 ri );
                Hata( Hata *php, Chiiki *p );
        qint32  x();
        qint32  y();
       QString  show();

        qint32  i;      // Goban position, as index
        qint32  ri;     // index of the Ryoiki in the Chiiki rpl this Hata is in
QPointer<Goban> bp;
};

/**
 * @brief The Ryoiki 領域 class - a region, defined as
 *   a group of Hata - like a Wyrm of free space.
 */
class Ryoiki : public QObject
{
    Q_OBJECT
public:
    explicit  Ryoiki( Chiiki *p );
        void  addHata( Hata *hp ) { if ( !hpl.contains( hp ) ) hpl.append( hp ); }
     QString  show();

                   qint32  color;  // Color of the surrounding Goishi, -1 if mixed
           QPointer<Goban> bp;
    QList<QPointer<Hata> > hpl;
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
        Hata *hata( qint32 i );
        bool  addHata( qint32 i, qint32 ri );
        bool  ryoikiListAdd( Hata * );
        void  hFill( qint32 x, qint32 y, qint32 ri );
        void  hCheck( int x, int y, int ri );
        bool  ryoikiColor( int x, int y, int *rc );
     QString  showRyoiki();

signals:

public slots:

public:
             QPointer<Goban> bp;
             QPointer<Shiko> tp;
    QVector<QPointer<Hata >> hGrid; // Hata, arranged on the Goban
    QList<QPointer<Ryoiki >> rpl;   // the same Hata, grouped in regions (like Wyrms of empty space)
};

#endif // CHIIKI_H
