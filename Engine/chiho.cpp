#include "chiho.h"

Chiho::Chiho( QObject *p ) : QObject(p)
{}

/**
 * @brief Chiho::Chiho - copy constructor
 * @param hp - Chiho to copy
 * @param p - Goban parent of the new Ryoiki
 */
Chiho::Chiho( Chiho *hp, QObject *p ) : QObject(p)
{ bi = hp->bi; }

