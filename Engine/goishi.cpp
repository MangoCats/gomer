#include "goishi.h"

Goishi::Goishi(qint32 c, Goban *p) : QObject(p), bp(p), color(c)
{ // qDebug( "Goishi constructor %d", color );
  x = GOSU_BOWL;
  y = c;
}

/**
 * @brief Goishi::Goishi - copy constructor
 * @param ip - Goishi to copy
 * @param p - Parent of the new Goishi
 */
Goishi::Goishi(Goishi *ip, Goban *p) : QObject(p),
               bp(p), color(ip->color), x(ip->x), y(ip->y)
{ // TODO: wp is recreated where?
}
