#ifndef GTPHANDLER_H
#define GTPHANDLER_H

class Game;
#include <QObject>
#include <QPointer>
#include "game.h"

/**
 * @brief The GtpHandler class - Engine interface for Go Text Protocol
 * http://www.lysator.liu.se/~gunnar/gtp/gtp2-spec-draft2/gtp2-spec.html
 * https://www.gnu.org/software/gnugo/gnugo_19.html#SEC192
 */
class GtpHandler : public QObject
{
    Q_OBJECT
public:
    explicit GtpHandler(Game *parent = nullptr);

signals:

public slots:

public:
    QPointer<Game> gp;
};

#endif // GTPHANDLER_H
