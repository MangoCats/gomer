#include "goishi.h"

Goishi::Goishi(qint32 c, Goban *parent) : QObject(parent), bp(parent), color(c)
{ // qDebug( "Goishi constructor %d", color );
  x = GOSU_BOWL;
  y = c;
}

/**
 * @brief Goishi::Goishi - copy constructor
 * @param ip - Goishi to copy
 * @param parent - Parent of the new Goishi
 */
Goishi::Goishi(Goishi *ip, Goban *parent) : QObject(parent),
               bp(parent), color(ip->color), x(ip->x), y(ip->y)
{ // TODO: wp is recreated where?
}
