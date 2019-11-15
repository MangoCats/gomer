#ifndef BUNKAI_H
#define BUNKAI_H

class Kogai;
class Ruikei;
class Shiko;
#include <QObject>
#include "kogai.h"
#include "ruikei.h"
#include "shiko.h"

#define MAX_PASS_IN_BRANCH 6

/**
 * @brief The Bunkai 分解 class - performs analysis of Ruikei, fills in their Kogai
 */
class Bunkai : public QObject
{
    Q_OBJECT
public:
        explicit  Bunkai( Shiko *p = nullptr );
           Kogai *moveOrPass( Ruikei *ap );
          qint32  playout( Ruikei *ap, qint32 i );

  QPointer<Shiko> tp;
};

#endif // BUNKAI_H
