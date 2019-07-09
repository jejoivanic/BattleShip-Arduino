#include <MaxMatrix.h>
#include "Coordinates.h"
#include "Animations.h"
#include "WordsUtils.h"

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

byte valueX = 0;
byte valueY = 0;
byte valueSlct = 0;

byte getXValue(Coordinate *coord);
void setXValue(Coordinate *coord, byte x);
byte getYValue(Coordinate *coord);
void setYValue(Coordinate *coord, byte y);

byte calculateNextPossiblePosition(Coordinate *positions[], byte (*getValueFunction)(Coordinate*), void (*setValueFunction)(Coordinate*, byte), bool atLeastOneMove) {
  byte temporalMovingAxis = (*getValueFunction)(nextPos);
  while (isMarkedPosition(positions, nextPos) || atLeastOneMove) {
    temporalMovingAxis = calculateNextIndex(temporalMovingAxis);
    (*setValueFunction)(nextPos, temporalMovingAxis);
    atLeastOneMove = false;
  }
}

void updateCurrentPos() {
  currentPos->setCoordinate(nextPos);
}

void initializePositions() {
  currentPos->setX(0);
  currentPos->setY(0);
  nextPos->setCoordinate(currentPos);
}

MaxMatrix* matrixes[] = {matrixPlayer1, matrixPlayer2};

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
  valueX = digitalRead(BTN0);
  valueY = digitalRead(BTN1);
  valueSlct = digitalRead(SLCT);

  if (valueX == HIGH) {
    calculateNextPossiblePosition(markedPositions, getXValue, setXValue, true);
  } else if (valueY == HIGH) {
    calculateNextPossiblePosition(markedPositions, getYValue, setYValue, true);
  } else if (valueSlct == HIGH) {
    Coordinate *newCoordinate = copyCoordinate(currentPos);
    markedPositions[placedShips++] = newCoordinate;
    setDot(matrix, currentPos, true);
    calculateNextPossiblePosition(markedPositions, getYValue, setYValue, true);
    markingMove = true;
  }

  if (shouldUpdate()) {
    if (markingMove) {
      markingMove = false;
    } else {
      setDot(matrix, currentPos, false);
    }
    setDot(matrix, nextPos, true);
    updateCurrentPos();
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
  valueX = digitalRead(BTN0);
  valueY = digitalRead(BTN1);
  valueSlct = digitalRead(SLCT);
  
  if (justChangedTurn) {
    matrixAwatingPlayer->clear();
    matrixAttackingPlayer->clear();
    drawCoordinatesInMatrix(matrixAwatingPlayer, markedPositionsAwaitingPlayer, availablePositionsAmount);
    drawCoordinatesInMatrix(matrixAttackingPlayer, attackedPositionsAttackingPlayer, attackingPositionsIndex);
    initializePositions();
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getYValue, setYValue, false);
    updateCurrentPos();
    justChangedTurn = false;
    delay(300);
  }
  if (valueX == HIGH) {
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getXValue, setXValue, true);
  } else if (valueY == HIGH) {
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getYValue, setYValue, true);
  } else if (valueSlct == HIGH) {
    Coordinate *newCoordinate = copyCoordinate(currentPos);
    attackedPositionsAttackingPlayer[attackingPositionsIndex++] = newCoordinate;
    bool hit = calculateAttackedPosition(currentPos, markedPositionsAwaitingPlayer, remainingShips);
    animation_fallingBomb(matrixes);
    if (hit) {
      animation_explotingBoat(matrixes);
      writeWordsSimultaneously(matrixAttackingPlayer, hitWord, hitWordLength, matrixAwatingPlayer, hitWord, hitWordLength);
    } else {
      writeWordsSimultaneously(matrixAttackingPlayer, missWord, missWordLength, matrixAwatingPlayer, missWord, missWordLength);
    }
    if (remainingShips > 0) {
      justChangedTurn = true;
      isFirstPlayerTurn = !isFirstPlayerTurn;
    } else {
      currentState = GAME_WON;
    }
  }

  if (shouldUpdate()) {
    setDot(matrixAttackingPlayer, currentPos, false);
    setDot(matrixAttackingPlayer, nextPos, true);
    updateCurrentPos();
  }
  blinkOnce(matrixAttackingPlayer);
}

void drawCoordinatesInMatrix (MaxMatrix* matrix, Coordinate* coords[], byte arraySize) {
  for(int i=0; i<arraySize; i++) {
    setDot(matrix, coords[i], true);
  }
}

bool calculateAttackedPosition(Coordinate *attackedCoord, Coordinate* otherPlayersShips[], byte &remainingShips) {
  for(int i=0; i<availablePositionsAmount; i++) {
    if (otherPlayersShips[i]->is(attackedCoord)) {
      otherPlayersShips[i] = NULL;
      remainingShips--;
      return true;
    }
  }
  return false;
}

/***********************
*  SHIP ATTACK STATE   *
***********************/

/***********************
*    GAME WON STATE    *
***********************/

void gameWon(MaxMatrix* matrixWinningPlayer, MaxMatrix* matrixLosingPlayer) {
  writeWordsSimultaneously(matrixWinningPlayer, winnerWord, winnerWordLength, matrixLosingPlayer, loserWord, loserWordLength);
}

/***********************
*    GAME WON STATE    *
***********************/
