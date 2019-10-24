#ifndef CHIHO_H
#define CHIHO_H

#include <QObject>
#include <QPointer>

/**
 * @brief The Chiho 地方 class - definition of a region on a
 *   Goban, the base class for more specific regions like
 *   Wyrm and Ryoiki.
 */
class Chiho : public QObject
{
    Q_OBJECT
public:
        explicit  Chiho( QObject *p = nullptr );
                  Chiho( Chiho *hp, QObject *p = nullptr );
            void  addGobanIndex( qint32 i ) { if ( !contains( i ) ) bi.append( i ); }
            void  clear() { bi.clear(); }
            bool  contains( qint32 i ) { return bi.contains(i); }
            void  join( Chiho *hp ) { foreach( qint32 i, hp->bi ) addGobanIndex( i ); }

    QList<qint32> bi;     // List of Goban index points in this Chiho
};


#endif // CHIHO_H
