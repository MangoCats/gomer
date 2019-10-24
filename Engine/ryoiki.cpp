#include "ryoiki.h"

/**
 * @brief Ryoiki::Ryoiki - a solidly connected set of points
 *   may be bounded by a single color of stones, or all stones.
 *   like a Wyrm of free space / liberties / opponent stones.
 * @param plp - Which player's Goishi form the border, if == total number of players then all Goishi border it
 * @param p   - Chiiki parent
 */
Ryoiki::Ryoiki( qint32 plp, Chiiki *p ) : Chiho(p->bp)
{ player = plp;       // Redundant with the pl storage index in the Chiiki, but not too costly to save here too.
  owner  = NO_PLAYER; // Unknown, at first, only has meaning player == total number of players
}

/**
 * @brief Ryoiki::Ryoiki - copy constructor
 * @param rp - Ryoiki to copy
 * @param p - Chiiki parent of the new Ryoiki
 */
Ryoiki::Ryoiki( Ryoiki *rp, Chiiki *p ) : Chiho(p->bp)
{ player = rp->player;
  owner  = rp->owner;
  wpl    = rp->wpl;
}
