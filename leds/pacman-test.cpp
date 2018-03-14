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
volatile int pacY = 4;
volatile int pacX = 4;

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

int collisionDetected(){
  switch (movdir) {
    case Left: return pacY - 1 < 0;
    case Right: return pacY + 1 >= MATY;
    case Down: return pacX - 1 < 0;
    case Up: return pacX + 1 >= MATX;
    default: BUG();
  }
}

void movePacman() {
  if (collisionDetected()) return;
    // we don't move coz we can't move

  switch (movdir) {
    case Left: pacY -= 1; break;
    case Right: pacY += 1; break;
    case Down: pacX -= 1; break;
    case Up: pacX += 1; break;
    default: BUG();
  }
}

void showPacman(int option) {
  matrix.drawPixel(pacX, pacY,
      colors[option == ERASE ? NONE : pacmanColor]);
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

void loop() {
  if (state == GameOver)
    gameOver();
  else if (state == Init) {
    unicolor(colors[NONE]);
    showPacman(0);
    matrix.show();
    state = Normal;
  }
  else {
    showPacman(ERASE); // 1 = erase pacman
    movePacman();
    showPacman(0);
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}


