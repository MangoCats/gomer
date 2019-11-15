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
            void  addIndex( qint32 i ) { if ( !contains( i ) ) il.append( i ); }
            void  clear() { il.clear(); }
            bool  contains( qint32 i ) { return il.contains(i); }
            void  join( Chiho *hp ) { foreach( qint32 i, hp->il ) addIndex( i ); }

    QList<qint32> il;     // List of index points in this Chiho
};


#endif // CHIHO_H
