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

// int pinLeft = 19;
// int pinRight = 3;
// int pinUp = 2;
// int pinDown = 18;
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
  NONE,
  WHITE,
};

const uint16_t colors[] = {
  matrix.Color(255, 0, 0),
  matrix.Color(0, 255, 0),
  matrix.Color(0, 0, 255),
  matrix.Color(255, 255, 0),
  matrix.Color(0, 255, 255),
  matrix.Color(255, 0, 255),
  matrix.Color(0, 0, 0),
  matrix.Color(255,255,255)
};


// int pacmanColor = BLUE;
// int wallColor = RED;
// int ghostColor = WHITE;
// int fruitColor = GREEN;
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

struct segment {
  int index; // line or column
  int horizontal;
  int pos; // position of head in line/column
  int size;
  int dir;
};

#define NSEGS 2
struct seg segments[NSEGS] = {
  { 0, 0, 0, 4, 1 },
  { MATX - 1, 1, 0, 5, -1 }
};

int mat[MATX][MATY];


void moveAll() {
  int i, j;
  int collided = 0;
  for (i = 0; i < NSEGS; i++) {
    struct seg *s = &segments[i];
    int nextpos = (s->pos + s->dir) % 8;
    int where = s->index;
    for (j = 0; j < NSEGS; j++) {
      if (collision(j, nextpos, s)) {
        if (collided) {
          collided = 0;
          continue; // we don't move it
        }
        else {
          s->dir = -s->dir;
          s->pos -= s->size*s->dir;
          collided = 1;
          i--;
          continue;
        }
      }
    }
    // if all ok:
    s->pos = (s->pos + s->dir) % 8;
    collided = 0;
  }
}

int collision(int indexSeg, int nextpos, struct seg *s) {
  Seg other = segments[indexSeg];
  if (other.horizontal != s->horizontal
    && other.index == nextpos) {
    if (other.dir) { 
      int otherMax = other.pos;
      int otherMin = (other.pos + other.size*other.dir) % 8;
    }
    else {
      int otherMin = other.pos;
      int otherMax = (other.pos + other.size*other.dir) % 8;
    }
    if (otherMax <= otherMin) {
      if ((otherMax - nextpos) >= 0 || (nextpos - otherMin) >= 0) {
        return 1; // collision
      }
    }
    else {
      if ((otherMax - nextpos) >= 0 && (nextpos - otherMin) >= 0) {
        return 1; // collision
      }
    }
  }
  return 0; // no collision
}

void showAll() {
  int i,j;
  for (i = 0; i < NSEGS; i++) {
    Seg s = segments[i];
    if (s.horizontal) {
      for (j = 0; j < s.size; j++) {
        matrix.drawPixel(s.index, (s.pos + j*s.dir) % 8, colors[i]);
      }
    }
    else {
      for (j = 0; j < s.size; j++) {
        matrix.drawPixel((s.pos + j*s.dir) % 8, s.index, colors[i]);
      }
    }
  }
}


void setup() {
  srand(time(NULL));
  matrix.begin();
  matrix.setBrightness(1);
}

void loop() {
  if (state == GameOver) {}
  else if (state == Init) {
    unicolor(colors[NONE]);
    showAll();
    state = Normal;
  }
  else { // Normal state
    moveAll();
    showAll();
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}

