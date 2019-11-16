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
            void  readRuikei();
            void  writeRuikei() const;
          qint32  moveOrPass( Ruikei *ap );
            void  playout( Ruikei *ap );
          Ruikei *matchingRuikei( Ruikei * );

                QString  ruikeiFilename;
         QPointer<Shiko> tp;
QList<QPointer<Ruikei> > apl;

};

#endif // BUNKAI_H
