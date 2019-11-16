#include "kogai.h"

Kogai::Kogai(Ruikei *p) : QObject(p)
{ passScore = SCORE_INVALID_MOVE;
  highScore = SCORE_INVALID_MOVE;
  bestMove  = MOVE_PASS_INDEX;
}

Kogai::Kogai(QDataStream &ds, Ruikei *p) : QObject(p)
{ quint8 version;
  ds >> version;
  if ( version != 1 )
    { qDebug( "Unknown version %d reading Kogai", version );
    }
   else
    { ds >> passScore;
      ds >> highScore;
      ds >> bestMove;

      qint32 ftlsz;
      ds >> ftlsz;
      for ( qint32 i = 0; i < ftlsz; i++ )
        ftl.append( new Soshi( ds, this ) );

      qint32 otlsz;
      ds >> otlsz;
      for ( qint32 i = 0; i < otlsz; i++ )
        otl.append( new Soshi( ds, this ) );
    }
}

void Kogai::toDataStream( QDataStream &ds ) const
{ quint8 version = 1;
  ds << version;
  ds << passScore;
  ds << highScore;
  ds << bestMove;

  qint32 ftlsz = ftl.size();
  ds << ftlsz;
  foreach ( Soshi *ep, ftl )
    ep->toDataStream( ds );

  qint32 otlsz = otl.size();
  ds << otlsz;
  foreach ( Soshi *ep, otl )
    ep->toDataStream( ds );
}

/**
 * @brief Kogai::copy
 * @param cop - pointer to Kogai which will have its contents copied into this one
 */
void Kogai::copy( Kogai *cop )
{ if ( cop == nullptr )
    { qDebug( "UNEXPECTED: Kogai::copy received nullptr" );
      return;
    }
  passScore = cop->passScore;
  highScore = cop->highScore;
  bestMove = cop->bestMove;
  ftl = cop->ftl;
  otl = cop->otl;
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
