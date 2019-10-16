#ifndef WYRM_H
#define WYRM_H

class Goishi;
class Shiko;
#include <QObject>
#include "goishi.h"
#include "shiko.h"

class Wyrm : public QObject
{
    Q_OBJECT
public:
    explicit Wyrm(Goishi *ipp, Shiko *parent = nullptr);
        void addGoishi( Goishi *ip );
        void addWyrm( Wyrm *wp );
      qint32 color();

signals:

public slots:

public:
  QList<QPointer<Goishi> > ipl;

};

#endif // WYRM_H
