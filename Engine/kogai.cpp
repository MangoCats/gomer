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

bool Kogai::isValid()
{ foreach ( Soshi *ep, ftl )
    if ( !ep->isValid() )
      return false;
  foreach ( Soshi *ep, otl )
    if ( !ep->isValid() )
      return false;
  return true;
}

QByteArray Kogai::toByteArray()
{ QByteArray ba;
  QDataStream ds( &ba, QIODevice::WriteOnly );

  qint32 ftlsz = ftl.size();
  ds << ftlsz;
  foreach ( Soshi *ep, ftl )
    ba.append( ep->toByteArray() );

  qint32 otlsz = otl.size();
  ds << otlsz;
  foreach ( Soshi *ep, otl )
    ba.append( ep->toByteArray() );

  return ba;
}

bool Kogai::fromByteArray( QByteArray ba )
{ foreach ( Soshi *ep, ftl )
    if ( ep != nullptr )
      ep->deleteLater();
  foreach ( Soshi *ep, otl )
    if ( ep != nullptr )
      ep->deleteLater();
  ftl.clear();
  otl.clear();
  QDataStream ds( &ba, QIODevice::WriteOnly );

  qint32 ftlsz;
  ds >> ftlsz;
  for ( qint32 i = 0; i < ftlsz; i++ )
    ftl.append( new Soshi( ds, this ) );

  qint32 otlsz;
  ds >> otlsz;
  for ( qint32 i = 0; i < otlsz; i++ )
    otl.append( new Soshi( ds, this ) );

  return isValid();
}
