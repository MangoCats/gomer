#ifndef GOSU_H
#define GOSU_H

class Goban;
class Goishi;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "goishi.h"

/**
 * @brief The Gosu class - holds stones not on the board.
 */
class Gosu : public QObject
{
    Q_OBJECT
public:
    explicit Gosu(Goban *parent = nullptr);
        void addGoishiToBowl(Goishi *p);

signals:

public slots:

public:
  QList<QPointer<Goishi> > bowl;  /// Stones yet to be played
  QList<QPointer<Goishi> > lid;   /// Stones captured
};

#endif // GOSU_H
