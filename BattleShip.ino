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

/* Characters */
const unsigned char A[] = {4, 8, B01111110, B00010001, B00010001, B01111110, B00000000};
const unsigned char B[] = {4, 8, B01111111, B01001001, B01001001, B00110110, B00000000};
const unsigned char C[] = {4, 8, B00111110, B01000001, B01000001, B00100010, B00000000};
const unsigned char D[] = {4, 8, B01111111, B01000001, B01000001, B00111110, B00000000};
const unsigned char E[] = {4, 8, B01111111, B01001001, B01001001, B01000001, B00000000};
const unsigned char F[] = {4, 8, B01111111, B00001001, B00001001, B00000001, B00000000};
const unsigned char G[] = {4, 8, B00111110, B01000001, B01001001, B01111010, B00000000};
const unsigned char H[] = {4, 8, B01111111, B00001000, B00001000, B01111111, B00000000};
const unsigned char I[] = {3, 8, B01000001, B01111111, B01000001, B00000000, B00000000};
const unsigned char J[] = {4, 8, B00110000, B01000000, B01000001, B00111111, B00000000};
const unsigned char K[] = {4, 8, B01111111, B00001000, B00010100, B01100011, B00000000};
const unsigned char L[] = {4, 8, B01111111, B01000000, B01000000, B01000000, B00000000};
const unsigned char M[] = {5, 8, B01111111, B00000010, B00001100, B00000010, B01111111};
const unsigned char N[] = {5, 8, B01111111, B00000100, B00001000, B00010000, B01111111};
const unsigned char O[] = {4, 8, B00111110, B01000001, B01000001, B00111110, B00000000};
const unsigned char P[] = {4, 8, B01111111, B00001001, B00001001, B00000110, B00000000};
const unsigned char Q[] = {4, 8, B00111110, B01000001, B01000001, B10111110, B00000000};
const unsigned char R[] = {4, 8, B01111111, B00001001, B00001001, B01110110, B00000000};
const unsigned char S[] = {4, 8, B01000110, B01001001, B01001001, B00110010, B00000000};
const unsigned char T[] = {5, 8, B00000001, B00000001, B01111111, B00000001, B00000001};
const unsigned char U[] = {4, 8, B00111111, B01000000, B01000000, B00111111, B00000000};
const unsigned char V[] = {5, 8, B00001111, B00110000, B01000000, B00110000, B00001111};
const unsigned char W[] = {5, 8, B00111111, B01000000, B00111000, B01000000, B00111111};
const unsigned char X[] = {5, 8, B01100011, B00010100, B00001000, B00010100, B01100011};
const unsigned char Y[] = {5, 8, B00000111, B00001000, B01110000, B00001000, B00000111};
const unsigned char Z[] = {4, 8, B01100001, B01010001, B01001001, B01000111, B00000000};

/* Words **/
const int hitWordLength = 3;
const unsigned char* hitWord[hitWordLength] = {H,I,T};
const int winnerWordLength = 6;
const unsigned char* winnerWord[winnerWordLength] = {W,I,N,N,E,R};
const int loserWordLength = 5;
const unsigned char* loserWord[loserWordLength] = {L,O,S,E,R};

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
  value0 = digitalRead(BTN0);
  value1 = digitalRead(BTN1);
  valueSlct = digitalRead(SLCT);

  if (value0 == HIGH) {
    calculateNextPossiblePosition(markedPositions, getXValue, setXValue, true);
  } else if (value1 == HIGH) {
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
  value0 = digitalRead(BTN0);
  value1 = digitalRead(BTN1);
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
  if (value0 == HIGH) {
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getXValue, setXValue, true);
  } else if (value1 == HIGH) {
    calculateNextPossiblePosition(attackedPositionsAttackingPlayer, getYValue, setYValue, true);
  } else if (valueSlct == HIGH) {
    Coordinate *newCoordinate = copyCoordinate(currentPos);
    attackedPositionsAttackingPlayer[attackingPositionsIndex++] = newCoordinate;
    bool hit = calculateAttackedPosition(currentPos, markedPositionsAwaitingPlayer, remainingShips);
    animation_fallingBomb(matrixes);
    if (hit) {
      animation_explotingBoat(matrixes);
      writeWordsSimultaneously(matrixAttackingPlayer, hitWord, hitWordLength, matrixAwatingPlayer, hitWord, hitWordLength);
      //writeWord(matrixAttackingPlayer, hitWord);
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

const unsigned char bomb[] = {8,8,
                              B00000000,
                              B00000000,
                              B01111101,
                              B10000110,
                              B10000110,
                              B01111101,
                              B00000000,
                              B00000000};

void animation_fallingBomb(MaxMatrix** matrixes) {
  writeSpriteAndDelay(matrixes, bomb, 500);
  int sizeOfMatrixes = sizeof(matrixes);
  for(int i=0; i<8; i++) {
    for(int m=0; m<sizeOfMatrixes; m++) {
      matrixes[m]->shiftDown(false);
    }
    delay(250);
  }
}

const unsigned char explotingBoat_0[] = {8,8,
  B00110000,
  B01010000,
  B10010000,
  B10010000,
  B10011100,
  B10010100,
  B10011100,
  B01110000};

const unsigned char explotingBoat_1[] = {8,8,
  B10000001,
  B01011010,
  B00100100,
  B01011010,
  B01011010,
  B00100100,
  B01011010,
  B10000001
};

const unsigned char explotingBoat_2[] = {8,8,
  B10111101,
  B01000010,
  B10011001,
  B10100101,
  B10100101,
  B10011001,
  B01000010,
  B10111101};

const unsigned char explotingBoat_3[] = {8,8,
  B10111101,
  B01000010,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B01000010,
  B10111101};

const unsigned char explotingBoat_4[] = {8,8,
  B10000001,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B10000001};

const unsigned char explotingBoat_5[] = {8,8,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000};

void animation_explotingBoat(MaxMatrix** matrixes) {
  writeSpriteAndDelay(matrixes, explotingBoat_0, 1000);
  writeSpriteAndDelay(matrixes, explotingBoat_1, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_2, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_3, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_4, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_5, 1000);
}

void writeSpriteAndDelay(MaxMatrix** matrixes, const unsigned char* sprite, int aDelay) {
  for(int i=0; i<sizeof(matrixes); i++) {
      matrixes[i]->writeSprite(0,0,sprite);
  }
  delay(aDelay);
}

void writeWord(MaxMatrix* matrix, const unsigned char** wordToWrite) {
  for(int i=0; i<=sizeof(wordToWrite); i++) {
    matrix->clear();
    delay(80);
    const unsigned char* character = wordToWrite[i];
    byte characterOffset = 3;
    matrix->writeSprite(2, 0, character);
    delay(500);
  }
}

void writeWordsSimultaneously(MaxMatrix* matrixA, const unsigned char** wordA, int wordALength, MaxMatrix* matrixB, const unsigned char** wordB, int wordBLength) {
  int longerWordLength = max(wordALength, wordBLength);

  for(int i=0; i<=longerWordLength; i++) {
    if (i<=wordALength) {
      matrixA->writeSprite(2,0,wordA[i]);
    }
    if (i<=wordBLength) {
      matrixB->writeSprite(2,0,wordB[i]);
    }
    delay(500);
    matrixA->clear();
    matrixB->clear();
    delay(80);
  }
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
