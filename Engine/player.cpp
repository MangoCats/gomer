#include "player.h"

Player::Player(Gosu *parent) : QObject(parent)
{ sp = parent;
  bp = sp->bp;
}
