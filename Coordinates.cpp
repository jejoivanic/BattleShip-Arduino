#include "Coordinates.h"

byte Coordinate::getX() {
  return this->x;
}

void Coordinate::setX(byte newX) {
  this->x = newX;
}

byte Coordinate::getY() {
  return this->y;
}

void Coordinate::setY(byte newY) {
  this->y = newY;
}

void Coordinate::setCoordinate(Coordinate *coord) {
  this->setX(coord->getX());
  this->setY(coord->getY());
}

bool Coordinate::is(byte x, byte y) {
  return this->x == x && this->y == y;
}

bool Coordinate::is(Coordinate *coord) {
  return this->is(coord->getX(), coord->getY());
}
