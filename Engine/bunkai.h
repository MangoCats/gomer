#ifndef BUNKAI_H
#define BUNKAI_H

class Ruikei;
#include <QObject>
#include "ruikei.h"

/**
 * @brief The Bunkai 分解 class - performs analysis of Ruikei, fills in their Kogai
 */
class Bunkai : public QObject
{
    Q_OBJECT
public:
    explicit  Bunkai( QObject *p = nullptr );
        void  predictTerritory( Ruikei *ap );
        bool  playout( Ruikei *ap, qint32 i );

signals:

public slots:
};

#endif // BUNKAI_H
