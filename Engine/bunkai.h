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

signals:

public slots:
};

#endif // BUNKAI_H
