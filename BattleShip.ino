#include <MaxMatrix.h>
#include "Coordinates.h"

/* Define Game States */ 
#define SHIP_PLACEMENT 0
#define SHIP_ATTACK    1
#define GAME_WON       2

#define availablePositionsAmount 4
#define matrixSideSize 8

/* Define Inputs */
#define BTN0 12
#define BTN1 13
#define SLCT 11

#define maxInUse 1

int currentState = SHIP_PLACEMENT;

bool markingMove = false;
bool isFirstPlayerTurn = true;
bool justChangedTurn = true;

Coordinate *currentPos = new Coordinate();
Coordinate *nextPos = new Coordinate();

/* Setup Player 1 */
#define DIN_player1 5   // DIN pin of MAX7219 module
#define CLK_player1 4   // CLK pin of MAX7219 module
#define CS_player1  2   // CS  pin of MAX7219 module
MaxMatrix *matrixPlayer1 = new MaxMatrix(DIN_player1, CS_player1, CLK_player1, maxInUse);
Coordinate *markedPositionsPlayer1[availablePositionsAmount];
Coordinate *attackedPositionsPlayer1[matrixSideSize * matrixSideSize];
int attackedPositionsPlayer1Index = 0;
byte remainingShipsPlayer1 = 0;

/* Setup Player 2 */
#define DIN_player2 9   // DIN pin of MAX7219 module
#define CLK_player2 8   // CLK pin of MAX7219 module
#define CS_player2  7   // CS  pin of MAX7219 module
MaxMatrix *matrixPlayer2 = new MaxMatrix(DIN_player2, CS_player2, CLK_player2, maxInUse);
Coordinate *markedPositionsPlayer2[availablePositionsAmount];
Coordinate *attackedPositionsPlayer2[matrixSideSize * matrixSideSize];
int attackedPositionsPlayer2Index = 0;
byte remainingShipsPlayer2 = 0;

byte value0 = 0;
byte value1 = 0;
byte valueSlct = 0;

byte getXValue(Coordinate *coord);
void setXValue(Coordinate *coord, byte x);
byte getYValue(Coordinate *coord);
void setYValue(Coordinate *coord, byte y);

byte calculateNextPossiblePosition(Coordinate *positions[], byte (*getValueFunction)(Coordinate*), void (*setValueFunction)(Coordinate*, byte)) {
  byte temporalMovingAxis = (*getValueFunction)(nextPos);
  do {
    temporalMovingAxis = calculateNextIndex(temporalMovingAxis);
    (*setValueFunction)(nextPos, temporalMovingAxis);
  } while (isMarkedPosition(positions, nextPos));
}

void initializePositions() {
  currentPos->setX(0);
  currentPos->setY(0);
  nextPos->setCoordinate(currentPos);
}

/* Game Initialization */
void setup() {
  Serial.begin(9600);
  pinMode(BTN0, INPUT);
  pinMode(BTN1, INPUT);
  pinMode(SLCT, INPUT);

  matrixPlayer1->init();
  matrixPlayer1->setIntensity(0);
  matrixPlayer1->clear();

  matrixPlayer2->init();
  matrixPlayer2->setIntensity(0);
  matrixPlayer2->clear();

  initializePositions();
}

void loop() {
  if (currentState == SHIP_PLACEMENT) {
    if (isFirstPlayerTurn) {
      shipPlacement(matrixPlayer1, markedPositionsPlayer1, remainingShipsPlayer1);
    } else {
      shipPlacement(matrixPlayer2, markedPositionsPlayer2, remainingShipsPlayer2);
    }
  } else if (currentState == SHIP_ATTACK) {
    if (isFirstPlayerTurn) {
      shipAttack(matrixPlayer1, attackedPositionsPlayer1, matrixPlayer2, markedPositionsPlayer2, attackedPositionsPlayer1Index, remainingShipsPlayer1);
    } else {
      shipAttack(matrixPlayer2, attackedPositionsPlayer2, matrixPlayer1, markedPositionsPlayer1, attackedPositionsPlayer2Index, remainingShipsPlayer2);
    }
  } else if (currentState == GAME_WON) {
    if (isFirstPlayerTurn) {
      gameWon(matrixPlayer1, matrixPlayer2);
    } else {
      gameWon(matrixPlayer2, matrixPlayer1);
    } 
  }
}

/***********************
* SHIP PLACEMENT STATE *
***********************/

void shipPlacement(MaxMatrix* matrix, Coordinate* markedPositions[], byte &placedShips) {
  value0 = digitalRead(BTN0);
  value1 = digitalRead(BTN1);
  valueSlct = digitalRead(SLCT);

  if (value0 == HIGH) {
    calculateNextPossiblePosition(markedPositions, getXValue, setXValue);
  } else if (value1 == HIGH) {
    calculateNextPossiblePosition(markedPositions, getYValue, setYValue);
  } else if (valueSlct == HIGH) {
    Coordinate *newCoordinate = copyCoordinate(currentPos);
    markedPositions[placedShips++] = newCoordinate;
    setDot(matrix, currentPos, true);
    calculateNextPossiblePosition(markedPositions, getYValue, setYValue);
    markingMove = true;
  }

  if (shouldUpdate()) {
    if (markingMove) {
      markingMove = false;
    } else {
      setDot(matrix, currentPos, false);
    }
    setDot(matrix, nextPos, true);
    currentPos->setCoordinate(nextPos);
  }
  if (! switchPlayerIfCorresponds(matrix, placedShips)) {
    blinkOnce(matrix);
  } else {
    delay(250);
  }
}

bool shouldUpdate() {
  return ! currentPos->is(nextPos);
}

void setDot(MaxMatrix* matrix, Coordinate *coord, bool value) {
  matrix->setDot(coord->getX(), coord->getY(), value);
}

void blinkOnce(MaxMatrix* matrix) {
  byte blinkingDelay = 80;
  setDot(matrix, currentPos, false);
  delay(blinkingDelay);
  setDot(matrix, currentPos, true);
  delay(blinkingDelay);
}

bool switchPlayerIfCorresponds(MaxMatrix *matrix, byte &placedShips) {
  if (placedShips >= availablePositionsAmount) {
    isFirstPlayerTurn = !isFirstPlayerTurn;
    setDot(matrix, currentPos, false);
    initializePositions();
    if (isFirstPlayerTurn) {
      currentState = SHIP_ATTACK;
    }
    return true;
  }
  return false;
}

boolean isMarkedPosition(Coordinate *positions[], Coordinate *coord) {
  return isMarkedPosition(positions, coord->getX(), coord->getY());
}

boolean isMarkedPosition(Coordinate *positions[], byte x, byte y) {
  for (int i=0; i<availablePositionsAmount; i++) {
    if (positions[i]->is(x, y)) {
      return true;
    }
  }
  return false;
}

byte getXValue(Coordinate *coord) {
  return coord->getX();
}

void setXValue(Coordinate *coord, byte x) {
  coord->setX(x);
}

byte getYValue(Coordinate *coord) {
  return coord->getY();
}

void setYValue(Coordinate *coord, byte y) {
  coord->setY(y);
}

byte calculateNextIndex (byte pos) {
  return (pos + 1) % matrixSideSize;
}

Coordinate* copyCoordinate(Coordinate *coord) {
  Coordinate *newCoordinate = new Coordinate();
  newCoordinate->setCoordinate(coord);
  return newCoordinate;
}

/***********************
* SHIP PLACEMENT STATE *
***********************/

/***********************
*  SHIP ATTACK STATE   *
***********************/

void shipAttack(MaxMatrix* matrixAttackingPlayer, Coordinate* attackedPositionsAttackingPlayer[], MaxMatrix* matrixAwatingPlayer, Coordinate* markedPositionsAwaitingPlayer[], int &attackingPositionsIndex, byte &remainingShips) {
  value0 = digitalRead(BTN0);
  value1 = digitalRead(BTN1);
  valueSlct = digitalRead(SLCT);
  
  if (justChangedTurn) {
    matrixAwatingPlayer->clear();
    matrixAttackingPlayer->clear();
    drawCoordinatesInMatrix(matrixAwatingPlayer, markedPositionsAwaitingPlayer, availablePositionsAmount);
    drawCoordinatesInMatrix(matrixAttackingPlayer, attackedPositionsAttackingPlayer, attackingPositionsIndex);
    initializePositions();
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getYValue, setYValue);
    justChangedTurn = false;
    delay(300);
  }
  if (value0 == HIGH) {
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getXValue, setXValue);
  } else if (value1 == HIGH) {
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getYValue, setYValue);
  } else if (valueSlct == HIGH) {
    Coordinate *newCoordinate = copyCoordinate(currentPos);
    attackedPositionsAttackingPlayer[attackingPositionsIndex++] = newCoordinate;
    calculateAttackedPosition(currentPos, markedPositionsAwaitingPlayer, remainingShips);
    if (remainingShips > 0) {
      justChangedTurn = true;
      Serial.println("Switch turn");
      isFirstPlayerTurn = !isFirstPlayerTurn;
    } else {
      currentState = GAME_WON;
    }
  }

  if (shouldUpdate()) {
    setDot(matrixAttackingPlayer, currentPos, false);
    setDot(matrixAttackingPlayer, nextPos, true);
    currentPos->setCoordinate(nextPos);
  }
  blinkOnce(matrixAttackingPlayer);
}

void drawCoordinatesInMatrix (MaxMatrix* matrix, Coordinate* coords[], byte arraySize) {
  for(int i=0; i<arraySize; i++) {
    setDot(matrix, coords[i], true);
  }
}

void calculateAttackedPosition(Coordinate *attackedCoord, Coordinate* otherPlayersShips[], byte &remainingShips) {
  for(int i=0; i<availablePositionsAmount; i++) {
    if (otherPlayersShips[i]->is(attackedCoord)) {
      otherPlayersShips[i] = NULL;
      remainingShips--;
    }
  }
}

/***********************
*  SHIP ATTACK STATE   *
***********************/

const unsigned char A[] = {8,8,B00000000,B00111100,B01100110,B01100110,B01111110,B01100110,B01100110,B01100110};
const unsigned char B[] = {8,8,B01111000,B01001000,B01001000,B01110000,B01001000,B01000100,B01000100,B01111100};
const unsigned char C[] = {8,8,B00000000,B00011110,B00100000,B01000000,B01000000,B01000000,B00100000,B00011110};
const unsigned char D[] = {8,8,B00000000,B00111000,B00100100,B00100010,B00100010,B00100100,B00111000,B00000000};
const unsigned char E[] = {8,8,B00000000,B00111100,B00100000,B00111000,B00100000,B00100000,B00111100,B00000000};
const unsigned char F[] = {8,8,B00000000,B00111100,B00100000,B00111000,B00100000,B00100000,B00100000,B00000000};
const unsigned char G[] = {8,8,B00000000,B00111110,B00100000,B00100000,B00101110,B00100010,B00111110,B00000000};
const unsigned char H[] = {8,8,B00000000,B00100100,B00100100,B00111100,B00100100,B00100100,B00100100,B00000000};
const unsigned char I[] = {8,8,B00000000,B00111000,B00010000,B00010000,B00010000,B00010000,B00111000,B00000000};
const unsigned char J[] = {8,8,B00000000,B00011100,B00001000,B00001000,B00001000,B00101000,B00111000,B00000000};
const unsigned char K[] = {8,8,B00000000,B00100100,B00101000,B00110000,B00101000,B00100100,B00100100,B00000000};
const unsigned char L[] = {8,8,B00000000,B00100000,B00100000,B00100000,B00100000,B00100000,B00111100,B00000000};
const unsigned char M[] = {8,8,B00000000,B00000000,B01000100,B10101010,B10010010,B10000010,B10000010,B00000000};
const unsigned char N[] = {8,8,B00000000,B00100010,B00110010,B00101010,B00100110,B00100010,B00000000,B00000000};
const unsigned char O[] = {8,8,B00000000,B00111100,B01000010,B01000010,B01000010,B01000010,B00111100,B00000000};
const unsigned char P[] = {8,8,B00000000,B00111000,B00100100,B00100100,B00111000,B00100000,B00100000,B00000000};
const unsigned char Q[] = {8,8,B00000000,B00111100,B01000010,B01000010,B01000010,B01000110,B00111110,B00000001};
const unsigned char R[] = {8,8,B00000000,B00111000,B00100100,B00100100,B00111000,B00100100,B00100100,B00000000};
const unsigned char S[] = {8,8,B00000000,B00111100,B00100000,B00111100,B00000100,B00000100,B00111100,B00000000};
const unsigned char T[] = {8,8,B00000000,B01111100,B00010000,B00010000,B00010000,B00010000,B00010000,B00000000};
const unsigned char U[] = {8,8,B00000000,B01000010,B01000010,B01000010,B01000010,B00100100,B00011000,B00000000};
const unsigned char V[] = {8,8,B00000000,B00100010,B00100010,B00100010,B00010100,B00010100,B00001000,B00000000};
const unsigned char W[] = {8,8,B00000000,B10000010,B10010010,B01010100,B01010100,B00101000,B00000000,B00000000};
const unsigned char X[] = {8,8,B00000000,B01000010,B00100100,B00011000,B00011000,B00100100,B01000010,B00000000};
const unsigned char Y[] = {8,8,B00000000,B01000100,B00101000,B00010000,B00010000,B00010000,B00010000,B00000000};
const unsigned char Z[] = {8,8,B00000000,B00111100,B00000100,B00001000,B00010000,B00100000,B00111100,B00000000};

/***********************
*    GAME WON STATE    *
***********************/

void gameWon(MaxMatrix* matrixWinningPlayer, MaxMatrix* matrixLosingPlayer) {
  int winnerWordLength = 6;
  const unsigned char* winnerWord[winnerWordLength] = {W,I,N,N,E,R};
  int loserWordLength = 5;
  const unsigned char* loserWord[loserWordLength] = {L,O,S,E,R};
  int longerWordLength = max(winnerWordLength, loserWordLength);

  for(int i=0; i<longerWordLength; i++) {
    matrixWinningPlayer->clear();
    matrixLosingPlayer->clear();
    delay(80);
    if (i<winnerWordLength) {
      matrixWinningPlayer->writeSprite(0, 0, winnerWord[i]);
    }
    if (i<loserWordLength) {
      matrixLosingPlayer->writeSprite(0, 0, loserWord[i]);
    }
    delay(500);
  }
}

/***********************
*    GAME WON STATE    *
***********************/
