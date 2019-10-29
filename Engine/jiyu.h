#ifndef JIYU_H
#define JIYU_H

class Goban;
class Shiko;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "shiko.h"

/**
 * @brief The Jiyu class - maps liberties of various levels
 *   Each point more distant from a Wyrm is a liberty of a higher
 *   number-level.  A single player's liberty map always shows
 *   the liberty of the lowest number for a grid point.
 *   No player's liberty of a higher number will be recorded
 *   on a grid point with an opponent liberty of a lower number.
 *   However, liberties of the same number for opposing players
 *   may be recorded on a single grid point.
 */
class Jiyu : public QObject
{
    Q_OBJECT
public:
    explicit  Jiyu(Goban *p);
              Jiyu(Jiyu *jp, Goban *p);
        void  clear();
        void  update();
        bool  check( qint32 i, qint32 x, qint32 y, qint32 level );
     QString  show();
        void  updateScore();
       qreal  score( qint32 c );

signals:

public slots:

public:
                   QPointer<Goban> bp;
    QVector<QPair<qint32,qint32> > grid; // First qint32 is the color, Second qint32 is the level
                    QVector<qreal> points;
};

#endif // JIYU_H
