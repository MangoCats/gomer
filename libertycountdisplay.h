#ifndef LIBERTYCOUNTDISPLAY_H
#define LIBERTYCOUNTDISPLAY_H

class BoardScene;
#include <QObject>
#include <QGraphicsSimpleTextItem>
#include "boardscene.h"

class LibertyCountDisplay : public QObject
{
    Q_OBJECT
public:
    explicit  LibertyCountDisplay( BoardScene *bsp = nullptr, QObject *parent = nullptr );

signals:

public slots:
        void  setShow( bool s ) { show = s; updateCounts(); }
        void  updateCounts();
        void  clearCounts();

public:
                           bool  show;
QList<QGraphicsSimpleTextItem *> counts;
            QPointer<BoardScene> sp;

};

#endif // LIBERTYCOUNTDISPLAY_H
