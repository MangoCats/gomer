#ifndef SOSHI_H
#define SOSHI_H

class Kogai;
#include <QObject>
#include <QDataStream>
#include "kogai.h"

/**
 * @brief The Soshi 素子 class - data elements of a Kogai
 */
class Soshi : public QObject
{
    Q_OBJECT
public:
    explicit  Soshi( Kogai *p = nullptr );
              Soshi( QDataStream &ds, Kogai *p = nullptr );
        void  toDataStream( QDataStream &ds ) const;
        bool  isValid() const;

      qint32  ntp;  // Number of Territory Points
      qint32  npm;  // Maximum number of Pass Moves associated with achieving ntp
};

#endif // SOSHI_H
