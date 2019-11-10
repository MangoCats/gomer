#ifndef KIGO_H
#define KIGO_H

#include <QObject>
#include <QDataStream>

/**
 * @brief The Kigo 記号 class - Indicates what may be present at a
 *   gridpoint of a Ruikei: off Goban, empty grid, goishi, etc.
 */
class Kigo : public QObject
{
    Q_OBJECT
public:
      explicit Kigo(QObject *parent = nullptr);
               Kigo(const Kigo&);
          void toDataStream( QDataStream &ds ) const;
        quint8 toByte() const;
          bool fromDataStream( QDataStream &ds );
          bool fromByte( quint8 );
          bool isValid() const;

          bool offGoban;       // this grid point is not on the Goban, may not hold Goishi
          bool emptyGrid;      // this gridpoint may be empty (can be combined with Goishi for don't care matching)
          bool friendlyGoishi; // friendly is the player who may place the next Goishi (or pass)
          bool opponentGoishi; // must wait for friendly's play before deciding their next move.
          bool ko;             // play at this gridpoint is not allowed in the coming turn
          bool alive;          // must be single Goishi, may connect off-Ruikei to a live Draco
          bool wyrm1;          // member of first Wyrm in analysis
          bool wyrm2;          // member of second Wyrm in analysis
};

#endif // KIGO_H
