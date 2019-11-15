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
          bool operator !=(const Kigo&) const;
          void invert();
          void toDataStream( QDataStream &ds ) const;
        quint8 toByte() const;
          bool fromDataStream( QDataStream &ds );
          bool fromByte( quint8 );
          bool isValid() const;
          bool isTerritory( bool friendly ) const;

          bool emptyGrid;              // this gridpoint may be empty (can be combined with Goishi for don't care matching)
          bool friendlyGoishi;         // friendly is the player who may place the next Goishi (or pass)
          bool opponentGoishi;         // must wait for friendly's play before deciding their next move.
          bool outsideFriendlyLiberty; // does the grid outside from this one have a friendly liberty (either empty or friendly Goishi with liberty?)
          bool outsideOpponentLiberty; // does the grid outside from this one have a friendly liberty (either empty or opponent Goishi with liberty?)
          bool ko;                     // play at this gridpoint is not allowed in the coming turn
          bool wyrm1;                  // member of first Wyrm in analysis
          bool wyrm2;                  // member of second Wyrm in analysis
};

#endif // KIGO_H
