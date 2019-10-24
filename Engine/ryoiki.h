#ifndef RYOIKI_H
#define RYOIKI_H

class Chiho;
class Chiiki;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "chiho.h"
#include "chiiki.h"
#include "wyrm.h"

/**
 * @brief The Ryoiki 領域 class - a Region, defined as a group
 *   of Goban intersection points - like a Wyrm of free space.
 */
class Ryoiki : public Chiho
{
    Q_OBJECT
public:
    explicit  Ryoiki( qint32 plp, Chiiki *p );
              Ryoiki( Ryoiki *rp, Chiiki *p );

                qint32  player; // Player's Goishi which bound this Ryoiki, if == total number of players then this Ryoiki borders any color Goishi
                qint32  owner;  // For Ryoiki which may border any Goishi, if only one color is bordered then this is the owner
 QList<QPointer<Wyrm> > wpl;    // All Wyrm which border this Ryoiki
};


#endif // RYOIKI_H
