#include "stone.h"

Stone::Stone( int px, int py, int pc, Board *pb, QObject *parent) : QObject(parent)
{ x = px;
  y = py;
  c = pc;
  board = pb;
}
