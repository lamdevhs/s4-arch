#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <stdlib.h>
#include <time.h>

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

#define STATIC 0

#define MATX 8
#define MATY 8

int pinLeft = 19;
int pinRight = 3;
int pinUp = 2;
int pinDown = 18;
int donnees= 13;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATX, MATY, donnees,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

enum {
  NONE,
  RED,
  GREEN,
  BLUE,
  YELLOW,
  CYAN,
  MAGENTA,
  WHITE,
};

const uint16_t colors[] = {
  matrix.Color(0, 0, 0),
  matrix.Color(255, 0, 0),
  matrix.Color(0, 255, 0),
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 0),
  matrix.Color(0, 255, 255),
  matrix.Color(255, 0, 255),
  matrix.Color(255,255,255)
};

int bugColor = MAGENTA;



// colors the whole matrix with one unique color
// useful for gameOver mode
// and to clear the whole matrix (using the color NONE)
void unicolor(uint16_t color) {
  int i, j;
  for (i = 0; i < MATX; i++) {
    for (j = 0; j < MATY; j++) {
      matrix.drawPixel(i, j, color);
    }
  }
}

typedef struct pix {
  int x;
  int y;
} Pix;

#define MAXMAP 8

enum
{
  GHOST,
  GHOST_FRUIT,
  FRUIT,
  NOTHING,
  WALL,
  PACMAN
};

int sizeMapX = MAXMAP;
int sizeMapY = MAXMAP;

int gameMap[MAXMAP][MAXMAP];


// general states of the game:
// Init = before the game (re)starts, to reset all the variables properly
enum States {
  Init, GameOver, Normal
};

enum DIR {
  Left,
  Up,
  Right,
  Down
};

enum {
  ERASE = 42
};


volatile int state = Init;

volatile int movdir = Right;

volatile int alreadyPressed = 0;
  // prevents a button from being pressed more than once
  // during one time unit of the game

#define FREQ 500
  // frequency of the game
  // aka time unit of the game, in ms

enum {
  WON, LOST
};

void gameOver(int hasWon) {
  unicolor(colors[hasWon == WON ? GREEN : RED]);
}

void BUG() {
  unicolor(colors[bugColor]);
}

void interr(int dir){
  if (alreadyPressed) return;
  if (state == GameOver)
    state = Init; // game over + button -> restart
  else movdir = dir;
  alreadyPressed = 1;
}

void iLeft() { interr(Left);}
void iRight(){ interr(Right);}
void iUp()   { interr(Up);}
void iDown() { interr(Down);}

int isMoveImpossible(int x, int y){
  if (x < 0 || y < 0 || x >= sizeMapX || y >= sizeMapY) {
    return 1;
  }
  int there = gameMap[x][y];
  if (there != FRUIT && there != NOTHING) return 1;
  return 0;
}


int movePos(int dir, int *x, int *y) {
  if (dir == Left) *y -= 1;
  if (dir == Right) *y += 1;
  if (dir == Down) *x -= 1;
  if (dir == Up) *x += 1;
}



enum {
  EMPTY = 0,
  FILLED = 1
};

int stack[MATX][MATY];

void newStack(){
  int i, j;
  for (i = 0; i < MATX; i++) {
    for (j = 0; j < MATY; j++) {
      stack[i][j] = NONE;
    }
  }
}

void showStack() {
  int i, j;
  for (i = 0; i < MATX; i++) {
    for (j = 0; j < MATY; j++) {
      matrix.drawPixel(i, j, stack[i][j]);
    }
  }
}
#define NPIECES 7

int pieces[NPIECES][6] = {
    // {1, 2, 3,
    //  4, 5, 6 },

// L
  {1, 1, 1,
   1, 0, 0 },
// SQUARE
  {1, 1, 0,
   1, 1, 0 },
// inverted L
  {1, 1, 1,
   0, 0, 1 },
// Line
  {1, 1, 1,
   0, 0, 0 },
// S
  {0, 1, 1,
   1, 1, 0 },
// Z
  {1, 1, 0,
   0, 1, 1 },
// T
  {1, 1, 1,
   0, 1, 0 }
};

int pieceX = 0;
int pieceY = 0;
int currentPiece[6] = {0,0,0,0,0,0};
int orient = 0;

void newPiece(int newPieceIx) {
  int i;
  for (i = 0; i < 6; i++) {
    currentPiece[i] = pieces[newPieceIx % NPIECES][i];
  }
  pieceX = MATX - 4;
  pieceY = MATY - 1;
  orient = 0;
}

void moveClockwise(int* piece, int orient) {
  int a = piece[0];
  int b = piece[1];
  int c = piece[2];
  int d = piece[3];
  int e = piece[4];
  int f = piece[5];
  if (orient % 2 == 0) {
    // [a b c]      [d a]
    // [d e f] ---> [e b]
    //              [f c]
    // cad
    // [a b c d e f] -> [d a e b f c]
    piece[0] = d;
    piece[1] = a;
    piece[2] = e;
    piece[3] = b;
    piece[4] = f;
    piece[5] = c;
  }
  else {
    // [a b]      [e c a]
    // [c d] ---> [f d b]
    // [e f]
    // cad
    // [a b c d e f] -> [e c a f d b]
    piece[0] = e;
    piece[1] = c;
    piece[2] = a;
    piece[3] = f;
    piece[4] = d;
    piece[5] = b;
  }
}

// counter-clockwise
void moveCClockwise(int *piece, int orient) {
  moveClockwise(piece, !!orient);
  moveClockwise(piece, !orient);
  moveClockwise(piece, !!orient);
}

void showPiece() {
  int color = 1;
  int i;
  if (orient % 2 == 0) {
    Serial.println(
      3000 +
      100*currentPiece[0] +
      10*currentPiece[1] +
      currentPiece[2]);
    Serial.println(
      3000 +
      100*currentPiece[3] +
      10*currentPiece[4] +
      currentPiece[5]);


    for (i = 0; i < 6; i++) {
      int y = i / 3;
      int x = i % 3;
      matrix.drawPixel(pieceX - x, pieceY - y, colors[currentPiece[i]*color]);
    }
  }
  else {
    Serial.println(
      300 +
      10*currentPiece[0] +
      currentPiece[1]);
    Serial.println(
      300 +
      10*currentPiece[2] +
      currentPiece[3]);
    Serial.println(
      300 +
      10*currentPiece[4] +
      currentPiece[5]);


    for (i = 0; i < 6; i++) {
      int y = i / 2;
      int x = i % 2;
      matrix.drawPixel(pieceX - x, pieceY - y, colors[currentPiece[i]*color]);
    }
  }
}

// void checkCollision(int orient) {
//   int color = 1+currentPiece;
//   int i;
//   int* pieceModel = pieces[currentPiece][orient];
//   if (orient % 2 == 0) {
//     for (i = 0; i < 6; i++) {
//       int y = i / 3;
//       int x = i % 3;
//       matrix.drawPixel(pieceX - x, pieceX - y, colors[pieceModel[i]*color]);
//     }
//   }
//   else {
//     for (i = 0; i < 6; i++) {
//       int y = i / 2;
//       int x = i % 2;
//       matrix.drawPixel(pieceX - x, pieceX - y, colors[pieceModel[i]*color]);
//     }
//   }
// }

void showAll() {
  showStack();
  showPiece(); 
}

void setup() {
  srand(time(NULL));
  pinMode(pinLeft, INPUT_PULLUP);
  pinMode(pinRight, INPUT_PULLUP);
  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinDown, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinLeft), iLeft, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinRight), iRight, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinUp), iUp, FALLING);
  attachInterrupt(digitalPinToInterrupt(pinDown), iDown, FALLING);
  matrix.begin();
  Serial.begin(9600);
  matrix.setBrightness(10);
}

int cpt = 0;

void loop() {
  if (state == GameOver) {}
  else if (state == Init) {
    unicolor(colors[GREEN]);
    newStack();
    newPiece(cpt++);
    showAll();
    state = Normal;
  }
  else { // Normal state
    /*
    unicolor(colors[NONE]);
    int theEnd = movePiece();
    showAll();
    if (theEnd) {
      gameOver(LOST);
      state = GameOver;
    }
    */
    moveClockwise(currentPiece, orient);
    orient = (orient + 1) % 4;
    unicolor(colors[GREEN]);
    showAll();
    if (orient == 0) {
      newPiece(++cpt);
    }
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}


