#include "kigo.h"

Kigo::Kigo(QObject *parent) : QObject(parent)
{      emptyGrid = true;
        offGoban = false;
  friendlyGoishi = false;
  opponentGoishi = false;
              ko = false;
           alive = false;
           wyrm1 = false;
           wyrm2 = false;
}

Kigo::Kigo(const Kigo&p) : QObject(p.parent())
{      emptyGrid = p.emptyGrid;
        offGoban = p.offGoban;
  friendlyGoishi = p.friendlyGoishi;
  opponentGoishi = p.opponentGoishi;
              ko = p.ko;
           alive = p.alive;
           wyrm1 = p.wyrm1;
           wyrm2 = p.wyrm2;
}

void Kigo::toDataStream( QDataStream &ds ) const
{ ds << toByte(); }

quint8 Kigo::toByte() const
{ return (  offGoban ? 0x01 : 0x0) |
    (      emptyGrid ? 0x02 : 0x0) |
    ( friendlyGoishi ? 0x04 : 0x0) |
    ( opponentGoishi ? 0x08 : 0x0) |
    (             ko ? 0x10 : 0x0) |
    (          alive ? 0x20 : 0x0) |
    (          wyrm1 ? 0x40 : 0x0) |
    (          wyrm2 ? 0x40 : 0x0);
}

bool Kigo::fromDataStream( QDataStream &ds )
{ quint8 b;
  ds >> b;
  return fromByte( b );
}

bool Kigo::fromByte( quint8 b )
{       offGoban = ((b & 0x01) != 0);
       emptyGrid = ((b & 0x02) != 0);
  friendlyGoishi = ((b & 0x04) != 0);
  opponentGoishi = ((b & 0x08) != 0);
              ko = ((b & 0x10) != 0);
           alive = ((b & 0x20) != 0);
           wyrm1 = ((b & 0x40) != 0);
           wyrm2 = ((b & 0x80) != 0);
  return isValid();
}

bool Kigo::isValid()
{ if ( offGoban ) // No other conditions allowed when offGoban is true
    { if ( emptyGrid      ||
           friendlyGoishi ||
           opponentGoishi ||
           ko             ||
           alive          ||
           wyrm1          ||
           wyrm2             )
        return false;
      return true;
    }
  if ( ko ) // ko must coexist with an empty grid, only
    { if ( !emptyGrid     ||
           friendlyGoishi ||
           opponentGoishi ||
           alive          ||
           wyrm1          ||
           wyrm2            )
        return false;
      return true;
    }
  if ( alive || wyrm1 || wyrm2 ) // alive, wyrm1 and wyrm2 must apply to a single color Goishi
    { if (( emptyGrid ) || ( friendlyGoishi && opponentGoishi ) || ( !friendlyGoishi && !opponentGoishi ))
        return false;
      return true;
    }
  // Other combinations are legal (don't cares)
  return true;
}
