#include "soshi.h"

Soshi::Soshi(Kogai *p) : QObject(p)
{ ntp = npm = -1; // Initalized as invalid.
}

Soshi::Soshi(QDataStream &ds, Kogai *p) : QObject(p)
{ ds >> ntp;
  ds >> npm;
}

/**
 * @brief Soshi::isValid
 * @return true if the member values are consistent with a valid Soshi
 */
bool Soshi::isValid()
{ if ( ntp < 0 )
    return false;
  if ( npm < 0 )
    return false;
  return true;
}

QByteArray Soshi::toByteArray()
{ QByteArray ba;
  QDataStream ds( &ba, QIODevice::WriteOnly );
  ds << ntp;
  ds << npm;
  return ba;
}
