#include <MaxMatrix.h>
#include "WordsUtils.h"

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
const unsigned char* hitWord[] = {H,I,T};
const int missWordLength = 4;
const unsigned char* missWord[] = {M,I,S,S};
const int winnerWordLength = 6;
const unsigned char* winnerWord[] = {W,I,N,N,E,R};
const int loserWordLength = 5;
const unsigned char* loserWord[] = {L,O,S,E,R};

/* Functions */
void writeWordsSimultaneously(MaxMatrix* matrixA, const unsigned char** wordA, int wordALength, MaxMatrix* matrixB, const unsigned char** wordB, int wordBLength) {
  int longerWordLength = max(wordALength, wordBLength);

  for(int i=0; i<longerWordLength; i++) {
    if (i<wordALength) {
      matrixA->writeSprite(2,0,wordA[i]);
    }
    if (i<wordBLength) {
      matrixB->writeSprite(2,0,wordB[i]);
    }
    delay(500);
    matrixA->clear();
    matrixB->clear();
    delay(80);
  }
}
