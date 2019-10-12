#ifndef GOISHI_H
#define GOISHI_H

#include <QObject>

/**
 * @brief The Goishi class - a stone
 */
class Goishi : public QObject
{
    Q_OBJECT
public:
    explicit Goishi(qint32 c, QObject *parent = nullptr);

signals:

public slots:

public:
  qint32 color; // 0 for black, 1 for white... might support additional colors
};

#endif // GOISHI_H
