#ifndef GTPHANDLER_H
#define GTPHANDLER_H

class Game;
#include <QCoreApplication>
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
    explicit  GtpHandler(QCoreApplication *app,Game *parent = nullptr);
        void  parseReceivedMessage( QString m, qint32 *id, QString *command_name, QString *arguments );
     QString  trimComments( QString m );
        void  respond( bool pf, qint32 id = -1, QString msg = "" );
        bool  checkGpNull( qint32 id );
        bool  checkBpNull( qint32 id );
        bool  checkTpNull( qint32 id );
      qint32  interpretColor( QString cs );

signals:
        void  response( QString );
        void  quit();

public slots:
        void  receivedMessage( QString );

public:
    QPointer<Game> gp;
      QStringList  handledCommands;
};

#endif // GTPHANDLER_H
