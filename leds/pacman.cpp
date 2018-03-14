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
  RED,
  GREEN,
  BLUE,
  YELLOW,
  CYAN,
  MAGENTA,
  NONE
};

const uint16_t colors[] = {
  matrix.Color(255, 0, 0),
  matrix.Color(0, 255, 0),
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 0),
  matrix.Color(0, 255, 255),
  matrix.Color(255, 0, 255),
  matrix.Color(0, 0, 0)
};


int pacmanColor = YELLOW;
int obstacleColor = BLUE;



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

#define MAXGHOSTS 6
#define MAXOBSTACLES 100


int sizeMapX = 10;
int sizeMapY = 10;
Pix ghosts[MAXGHOSTS];
int nghosts = 0;

Pix obstacles[MAXOBSTACLES];
int nobstacles = 0;
volatile int pacY = 1;
volatile int pacX = 1;

#define PACX_REAL 4
#define PACY_REAL 4


void buildMap() {
  int i;
  nobstacles = 0;
  for (i = 0; i < sizeMapX && nobstacles < MAXOBSTACLES; i++) {
    obstacles[nobstacles].x = i;
    obstacles[nobstacles].y = 0;
    ++nobstacles;
  }
  for (i = 1; i < sizeMapY - 1 && nobstacles < MAXOBSTACLES; i++) {
    obstacles[nobstacles].x = 0;
    obstacles[nobstacles].y = i;
    ++nobstacles;
  }
}


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


// prints a very angry, very red matrix
// of very angry red leds
void gameOver() {
  unicolor(colors[RED]);
}

void BUG() {
  unicolor(colors[BLUE]);
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

int collisionDetected(int x, int y){
  if (x < 0 || y < 0 || x >= sizeMapX || y >= sizeMapY) {
    return 1;
  }

  int i = 0;
  for (i = 0; i < nobstacles; i++) {
    if (obstacles[i].x == x && obstacles[i].y == y) {
      return 1; 
    }
  }
  return 0;
}


int movePos(int dir, int *x, int *y) {
  if (dir == Left) *y -= 1;
  if (dir == Right) *y += 1;
  if (dir == Down) *x -= 1;
  if (dir == Up) *x += 1;
}

void movePacman() {
  int tempX = pacX;
  int tempY = pacY;
  movePos(movdir, &tempX, &tempY);

  if (collisionDetected(tempX, tempY)) return;
    // we don't move coz we can't move

  pacX = tempX;
  pacY = tempY;
}

void showAll() {
  int i;
  for (i = 0; i < nobstacles; i++) {
    int realX = realPosX(obstacles[i].x);
    int realY = realPosY(obstacles[i].y);
    if (realX != -1 && realY != -1) {
      matrix.drawPixel(realX, realY,
        colors[obstacleColor]);
    }

  }
  matrix.drawPixel(PACX_REAL, PACY_REAL,
    colors[pacmanColor]);
}

int realPosX(int x) {
  int val = x - pacX + PACX_REAL;
  if (val < 0 || val >= MATX) return -1;
  else return val;
}
int realPosY(int y) {
  int val = y - pacY + PACY_REAL;
  if (val < 0 || val >= MATY) return -1;
  else return val;
}

void setup() {
  srand(time(NULL));
  buildMap();
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

void loop() {
  if (state == GameOver)
    gameOver();
  else if (state == Init) {
    unicolor(colors[NONE]);
    showAll();
    matrix.show();
    state = Normal;
  }
  else {
    unicolor(colors[NONE]);
    movePacman();
    showAll();
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}


