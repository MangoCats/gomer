#ifndef RUIKEI_H
#define RUIKEI_H

class Goban;
class Kigo;
class Kogai;
class Shiko;
class Menseki;
class Wyrm;
#include <QObject>
#include <QPointer>
#include "goban.h"
#include "kigo.h"
#include "kogai.h"
#include "shiko.h"
#include "menseki.h"
#include "wyrm.h"

#define MOVE_PASS_INDEX      -1
#define MOVE_UNDEFINED_INDEX -2

/**
 * @brief The Ruikei 類型 class - a pattern for matching with pre-calculated
 *   analysis of the situation depicted.
 */
class Ruikei : public Menseki
{
    Q_OBJECT
public:
    explicit  Ruikei( qint32 xs, qint32 ys, Shiko *p = nullptr );
              Ruikei( QDataStream &ds, Shiko *p = nullptr );
              Ruikei( Ruikei *pap, qint32 i );
        void  toDataStream( QDataStream &ds ) const;
        bool  isValid() const;
      qint32  playAt( qint32 i );
      qint32  attemptCapture( qint32 x, qint32 y, Chiho &h );
        void  swapFriendlyOpponent();
        bool  legalFriendlyMove( qint32 i );
        bool  matchWyrm( Wyrm *wp, Goban *bp, qint32 color );
        bool  matchChihoInCurrentOrientation( Chiho *hp, Goban *bp );
        bool  matchInPosition( Goban *bp );
        bool  matchOneGoishi( Goishi *ip, const Kigo &k );
        bool  matchBoth( Wyrm *wp1, Wyrm *wp2, Goban *bp );
        bool  matchPosition( Ruikei *ap );
      qint32  nPreviousPass();
      qint32  nEdges() const;
      qint32  nXedges() const;
      qint32  nYedges() const;
        bool  cornerEdges() const;
        bool  x0Edge() const;
        bool  y0Edge() const;
        bool  xSizeEdge() const;
        bool  ySizeEdge() const;
        Kigo  kigoAt( qint32 x, qint32 y );
      qint32  nGoishi( bool friendly );
      qint32  nCaptured( bool friendly );
      qint32  nTerritory( bool friendly ); // TODO: Benson's life
      qint32  score();                     // score value evaluated at final position

public:
   QVector<Kigo> kl;            // What's on each grid point?
           bool  nEdge;         // North border is Ysize in orientation 0
           bool  eEdge;         // East  border is Xsize in orientation 0
           bool  wEdge;         // West  border is X -1  in orientation 0
           bool  sEdge;         // South border is Y -1  in orientation 0

// Data for analysis of the Ruikei, instead of definition of it
 QPointer<Shiko> tp;            // Shiko this Ruikei will be evauated by
 QPointer<Kogai> op;            // Synopsis/outline of what is known about this situation, looking down all branches
QPointer<Ruikei> ap;            // Previous position (for deep analysis trees)
         qint32  xo,yo;         // Offset into the Goban where this Ruikei is matched (at "orientation")
         qint32  friendlyColor; // Color of Goishi to consider "friendly"
         qint32  depth;         // How many plays have been made since the "top"?
         qint32  previousMove;  // What (opponent) move led to this Ruikei position
         qint32  justCaptured;  // How many Goishi did the previous move capture?
};

#endif // RUIKEI_H

/* Pattern matching thoughts: (Ruikei 類型)
 * Every grid point in a pattern may be in one of five states:
 *  1) empty
 *  2) player's Goishi (player who has the next move)
 *  3) opponent's Goishi
 *  4) off-board (typically arrayed as an edge line or corner in the pattern)
 *  5) alive (member of a live Draco which may extend outside the pattern)
 *  6) Ko 児 (play at this position in the coming turn would result in Ko)
 *  7) first Wyrm - is this Goishi a member of the "first" Wyrm for analysis
 *  8) second Wyrm - is this Goishi a member of the "second" Wyrm for analysis
 *
 * For purposes of pattern matching, grid points may be defined as
 *   "don't care" combinations of any/all of the first 4 states, at
 *   least one of the first 4 states must be true.  The 5 alive state
 *   may only be true if one or both of the 2 or 3 states is true.
 *   The 6 Ko state may only be true if the 1 empty state is true, and
 *   it may only be true for a single point in a pattern at any time.
 *
 *   At least one point must be labeled with one of the 7 or 8 labels.
 *   Only points labeled with 2 or 3 may also carry 7 or 8 labels.
 *   In a given pattern, all points labeled with 7 or 8 must be labeled
 *   with all 2, or all 3, never both (don't care).
 *   When any point is labeled with 7 and/or 8, all other connected points
 *   (in the same Wyrm) must also be labeled with the same 7 or 8.
 *   If one point is labeled with 8, at least one other disconnected point
 *   must also be labeled with 8.
 *
 *   Normally, a pattern would only store 7, or 8 information not both, but
 *   there is nothing in the data structure preventing both from being present.
 *
 * A pattern is stored as a string of bytes, first byte being the number
 *   of columns in the pattern, second byte being the number of rows,
 *   N bytes one per grid point in the pattern's rectangle, followed by
 *   2 bytes telling about connectability of any state 8 Wyrms and
 *   2 more bytes telling about capturability of any state 7 Wyrms
 *
 * The minimum moves bytes encode either: 0 meaning no information or
 *   an integer 1-255 (usually less than 8) which indicates the minimum
 *   number data for the info point.
 *
 * Although AlphaZero may have recently made it fashionable to eschew
 *   the 8 pattern rotations and reflections, I consider this a development
 *   in the opposite direction which can gain increased benefit from a
 *   single pattern.  If a pattern matches in any of the 8 rotations /
 *   reflections, then it's pre-learned wisdom applies:
 *
 * Patterns may be used to establish:
 *  - connectability of two Wyrms
 *   - minimum number of turns required to connect two Wyrms when
 *     faced with optimally played active opposition to connection
 *     - or with opponent passing once, twice, etc.
 *   - minimum number of turns required to prevent connection of two Wyrms when
 *     faced with optimally played active attempt at connection
 *     - or with opponent passing once, twice, etc.
 *   - indeterminate state, insufficient information (or knowledge) in the
 *     pattern to say whether the Wyrms can be certainly connected or cut.
 *
 *  - capture / territory securing
 *   - minimum number of turns required to capture a Wyrm when
 *     faced with optimally played active opposition to capture
 *     - with one opponent pass, two, three, etc.
 *   - minimum number of turns required to render a Wyrm "safe" when
 *     faced with optimally played active attempt to capture.
 *     - amount of territory secured by the Wyrm when "alive"
 *     - with one oppnent pass, two, three, etc.
 *   - indeterminate state, insufficient information (or knowledge) in the
 *     pattern to say whether the Wyrm can be certainly captured or
 *     rendered safe.
 *
 * When is a Wyrm "safe"?
 *  - when it is pass-alive or
 *  - when it has secure connection in its Draco to a Wyrm
 *       that is pass-alive
 *  - when it has a single eye that is securely convertable to two eyes
 *
 * When is a connection secure?
 *   - when there is a known minimum number of turns required to connect
 *     when faced with optimally played active opposition to connection
 *
 * Ladder (Hashigo 梯子) analysis:
 *   When a Goishi may be placed in Atari, but that Atari can be escaped
 *   to 2 Liberties, and the resulting 2 Liberties may be reduced to an
 *   Atari, which again may be escaped to 2 Liberties, this is a ladder
 *   situation.  Ladders may end with the threatened Wyrm being ultimately
 *   captured, or ultimately escaping to 3 or more Liberties.  Once 3 Liberties
 *   are achieved, the threatening Wyrms may (or may not) have vulnerabilities
 *   (such as single liberties which, when taken, result in multiple Ataris)
 *   which may be used to effectively release the threatened Wyrm.
 *
 * Ladders defy local pattern analysis and must "run the board" until they
 *   have resulted in either capture or release of the threatened Wyrm.
 *
 * The interesting question answered by ladder analysis is: what does the
 *   board look like after the ladder has run to completion?
 */

