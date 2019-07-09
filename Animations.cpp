#include <MaxMatrix.h>

/* Animations */
const unsigned char bomb[] = {8,8,
                              B00000000,
                              B00000000,
                              B01111101,
                              B10000110,
                              B10000110,
                              B01111101,
                              B00000000,
                              B00000000};

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

/* Functions */
void writeSpriteAndDelay(MaxMatrix** matrixes, const unsigned char* sprite, int aDelay) {
  for(int i=0; i<sizeof(matrixes); i++) {
      matrixes[i]->writeSprite(0,0,sprite);
  }
  delay(aDelay);
}

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

void animation_explotingBoat(MaxMatrix** matrixes) {
  writeSpriteAndDelay(matrixes, explotingBoat_0, 800);
  writeSpriteAndDelay(matrixes, explotingBoat_1, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_2, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_3, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_4, 250);
  writeSpriteAndDelay(matrixes, explotingBoat_5, 500);
}
