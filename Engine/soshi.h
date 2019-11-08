#ifndef SOSHI_H
#define SOSHI_H

class Kogai;
#include <QObject>
#include <QDataStream>
#include "kogai.h"

class Soshi : public QObject
{
    Q_OBJECT
public:
    explicit Soshi(Kogai *p = nullptr);
             Soshi(QDataStream &ds, Kogai *p = nullptr);
  QByteArray toByteArray();
        bool isValid();

      qint32 ntp;  // Number of Territory Points
      qint32 npm;  // Maximum number of Pass Moves associated with achieving ntp
};

#endif // SOSHI_H
