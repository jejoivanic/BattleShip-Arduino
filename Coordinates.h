#include <Arduino.h>

class Coordinate {
  private:
    short x = 0;
    short y = 0;
  public:
    byte getX();
    void  setX(byte newX);
    byte getY();
    void  setY(byte newY);
    void setCoordinate(Coordinate *coord);
    bool is(byte x, byte y);
    bool isX(byte x);
    bool isY(byte y);
    bool is(Coordinate *coord);
};
