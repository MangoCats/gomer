#ifndef GOISHI_H
#define GOISHI_H

class Goban;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "wyrm.h"

// X coordinates when in Gosu, Y coordinate is the color of the Gosu owner
#define GOSU_BOWL -2
#define GOSU_LID  -3

/**
 * @brief The Goishi class - a stone
 */
class Goishi : public QObject
{
    Q_OBJECT
public:
    explicit Goishi(qint32 c, Goban *parent = nullptr);

signals:

public slots:

public:
    QPointer<Goban> bp;
     QPointer<Wyrm> wp;
            qint32  color; // 0 for black, 1 for white... might support additional colors
            qint32  x,y;   // Coordinates - x can be negative, -2 when in Gosu bowl or -3 in lid
};

#endif // GOISHI_H
