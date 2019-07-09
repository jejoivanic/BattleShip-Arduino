#include <MaxMatrix.h>

/* Characters */
extern const unsigned char A[];
extern const unsigned char B[];
extern const unsigned char C[];
extern const unsigned char D[];
extern const unsigned char E[];
extern const unsigned char F[];
extern const unsigned char G[];
extern const unsigned char H[];
extern const unsigned char I[];
extern const unsigned char J[];
extern const unsigned char K[];
extern const unsigned char L[];
extern const unsigned char M[];
extern const unsigned char N[];
extern const unsigned char O[];
extern const unsigned char P[];
extern const unsigned char Q[];
extern const unsigned char R[];
extern const unsigned char S[];
extern const unsigned char T[];
extern const unsigned char U[];
extern const unsigned char V[];
extern const unsigned char W[];
extern const unsigned char X[];
extern const unsigned char Y[];
extern const unsigned char Z[];

/* Words **/
extern const int hitWordLength;
extern const unsigned char* hitWord[];
extern const int missWordLength;
extern const unsigned char* missWord[];
extern const int winnerWordLength;
extern const unsigned char* winnerWord[];
extern const int loserWordLength;
extern const unsigned char* loserWord[];

/* Functions */
void writeWordsSimultaneously(MaxMatrix* matrixA, const unsigned char** wordA, int wordALength, MaxMatrix* matrixB, const unsigned char** wordB, int wordBLength);
