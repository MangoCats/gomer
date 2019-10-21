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
    explicit  Wyrm( Goishi *ip, Shiko *p );
              Wyrm(   Wyrm *wp, Shiko *p );
        void  addGoishi( Goishi *ip );
        void  mergeLibertyList( QList<qint32> );
QList<qint32> getLibertyList( Goishi *ip );
        void  addWyrm( Wyrm *wp );
      qint32  color();
        bool  removeLiberty( qint32 );
        bool  addLiberty( qint32 );
        bool  inAtari() { return ( libertyList.size() == 1 ); }
     QString  show();

signals:

public slots:

public:
  QList<QPointer<Goishi> > ipl;          // List of all Goishi in this Wyrm
             QList<qint32> libertyList;  // List of all this Wyrm's liberties, by grid index

};

#endif // WYRM_H
