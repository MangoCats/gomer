#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QPointer>
#include "goban.h"
#include "gosu.h"

/**
 * @brief The Game class - encapsulates a single game play
 */
class Game : public QObject
{
    Q_OBJECT
public:
    explicit Game(qint32 nPlayers = 2, QObject *parent = nullptr);

signals:

public slots:

public:
         QPointer<Goban> bp;
  QList<QPointer<Gosu> > spl;

};

#endif // GAME_H
