#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>

int donnees= 6;

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

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, donnees,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 0), matrix.Color(0, 255, 255), matrix.Color(255, 0, 255)};

void blank() {
  int i, j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      matrix.drawPixel(i, j, 0);
    }
  }
}

typedef struct pix {
  int x;
  int y;
} Pix;

typedef int[2][3] Piece;

Piece pieces[7] = {
  {{1,1,0},
   {1,1,0}},
  {{1,1,1},
   {0,1,0}},
  {{1,0,0},
   {0,0,0}},
  {{1,1,1},
   {0,0,0}},
  {{1,1,0},
   {0,0,0}},
  {{1,1,0},
   {0,1,1}},
  {{0,1,1},
   {1,1,0}}
};

Piece *current = pieces;

enum States {
  Init, GameOver, Normal
};
volatile int state = Init;

enum DIR {
  LEFT,
  TOP,
  RIGHT,
  BOTTOM
};

volatile int dir = RIGHT;
volatile int alreadyPressed = 0;

#define FREQ 500

void gameOver() {
  int i, j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      matrix.drawPixel(i, j, colors[0]);
    }
  }
}

void btnClockwise(){
  if (alreadyPressed) return;
  if (state == GameOver) state = Init;
  else dir = (dir + 1) % 4;
  alreadyPressed = 1;
  
}

void btnCounterClockwise(){
  if (alreadyPressed) return;
  if (state == GameOver) state = Init;
  else dir = (dir + 3) % 4;
  alreadyPressed = 1;
}

void printPiece() {
  
}

void setup() {
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(0, btnClockwise, FALLING);
  attachInterrupt(1, btnCounterClockwise, FALLING);
  matrix.begin();
  matrix.setBrightness(10);
  
}

void loop() {
  if (state == GameOver) gameOver();
  else if (state == Init) {
    blank();
    
    matrix.show();
    state = Normal;
  }
  else {
    
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}
