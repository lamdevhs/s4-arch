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
int donnees= 53;

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
  matrix.setBrightness(10);
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
#define NPIECES 6

int pieces[NPIECES][4][6] = {
  // L
  {
    {1, 1, 1,
     1, 0, 0 },

    {1, 0,
     1, 0,
     1, 1 },

    {0, 0, 1,
     1, 1, 1 },

    {1, 1,
     0, 1,
     0, 1 }
  },
  // square
  {
    {1, 1, 0,
     1, 1, 0 },

    {1, 1,
     1, 1,
     0, 0 },

    {1, 1, 0,
     1, 1, 0 },

    {1, 1,
     1, 1,
     0, 0 }
  }
};

int pieceX = 0;
int pieceY = 0;
int currentPiece = 0;
int orient = 0;

void newPiece() {
  currentPiece = 0;
  pieceX = MATX - 1;
  pieceY = MATY - 1;
  orient = 0;
}


void showPiece() {
  int color = 1+currentPiece;
  int i;
  int* pieceModel = pieces[currentPiece][orient];
  if (orient % 2 == 0) {
    for (i = 0; i < 6; i++) {
      int x = i / 3;
      int y = i % 3;
      matrix.drawPixel(pieceX - x, pieceX - y, colors [pieceModel[0]*color]);
    }
  }
}

void showAll() {
  showStack();
  showPiece();
  
}

void loop() {
  if (state == GameOver) {}
  else if (state == Init) {
    unicolor(colors[GREEN]);
    newStack();
    newPiece();
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
    unicolor(colors[RED]);
    newStack();
    newPiece();
    showAll();
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}


