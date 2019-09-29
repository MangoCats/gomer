#ifndef GAMECONTROL_H
#define GAMECONTROL_H

#include <QObject>

class GameControl : public QObject
{
    Q_OBJECT
public:
    explicit  GameControl(QObject *parent = nullptr);
        void  start();

signals:
    void stateMessage( QString );

public slots:
};

#endif // GAMECONTROL_H
