#include "Location.h"



Location::Location() : x(0), y(0) {}

Location::Location(int xp, int yp) : x(xp), y(yp) {}


int Location::getX() const { return x; }
void Location::setX(int xp) { x = xp; }

int Location::getY() const { return y; }
void Location::setY(int yp) { x = yp; }