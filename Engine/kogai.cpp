#include "kogai.h"

Kogai::Kogai(Ruikei *p) : QObject(p)
{}

Kogai::Kogai(QDataStream &ds, Ruikei *p) : QObject(p)
{ qint32 ftlsz;
  ds >> ftlsz;
  for ( qint32 i = 0; i < ftlsz; i++ )
    ftl.append( new Soshi( ds, this ) );

  qint32 otlsz;
  ds >> otlsz;
  for ( qint32 i = 0; i < otlsz; i++ )
    otl.append( new Soshi( ds, this ) );
}

void Kogai::toDataStream( QDataStream &ds ) const
{ qint32 ftlsz = ftl.size();
  ds << ftlsz;
  foreach ( Soshi *ep, ftl )
    ep->toDataStream( ds );

  qint32 otlsz = otl.size();
  ds << otlsz;
  foreach ( Soshi *ep, otl )
    ep->toDataStream( ds );
}

/**
 * @brief Kogai::isValid
 * @return true if all Soshi are valid
 */
bool Kogai::isValid() const
{ foreach ( Soshi *ep, ftl )
    if ( !ep->isValid() )
      return false;
  foreach ( Soshi *ep, otl )
    if ( !ep->isValid() )
      return false;
  return true;
}

/**
 * @brief Kogai::isEmpty
 * @return true if there is no data in the lists
 */
bool Kogai::isEmpty() const
{ if (( ftl.size() < 1 ) && ( otl.size() < 1 ))
    return true;
  return false;
}
