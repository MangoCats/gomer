#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QObject>
#include <QPointF>

class GameControl : public QObject
{
    Q_OBJECT
public:
    explicit  GameControl(QObject *parent = nullptr);
        void  start();

signals:
        void  stateMessage( QString );

public slots:
        void  clickAt( QPointF );
};

#endif // GAMECONTROL_H
