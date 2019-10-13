#ifndef GOISHI_H
#define GOISHI_H

class Goban;
#include <QObject>
#include <QPointer>
#include "goban.h"

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
            qint32  color; // 0 for black, 1 for white... might support additional colors
};

#endif // GOISHI_H
