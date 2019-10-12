#ifndef GOBAN_H
#define GOBAN_H

#include <QObject>

/**
 * @brief The Goban class - a generalized playing board
 */
class Goban : public QObject
{
    Q_OBJECT
public:
    explicit  Goban(QObject *parent = nullptr, qint32 xs=19, qint32 ys=19);
      qint32  nPoints() { return Xsize * Ysize; }

signals:

public slots:

public:
    qint32 Xsize,Ysize;
};

#endif // GOBAN_H
