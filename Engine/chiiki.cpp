#include "chiiki.h"

Chiiki::Chiiki(Shiko *p) : QObject(p), bp(p->bp), tp(p)
{}

/**
 * @brief Chiiki::Chiiki - copy constructor
 * @param pcp - pointer to the Chiiki this is making a copy of
 * @param p - Shiko parent of the new Chiiki
 */
Chiiki::Chiiki(Chiiki *pcp,Shiko *p) : QObject(p), bp(pcp->bp), tp(p)
{}
