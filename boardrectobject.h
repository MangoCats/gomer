#ifndef BOARDRECTOBJECT_H
#define BOARDRECTOBJECT_H

#include <QObject>
#include <QRectF>

class BoardRectObject : public QObject
{
    Q_OBJECT
public:
    explicit  BoardRectObject(QObject *parent = nullptr);
        void  clickAt( QPointF );

signals:
        void  clicked( QPointF );

public slots:
};

#endif // BOARDRECTOBJECT_H
