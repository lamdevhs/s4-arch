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

int pos = 0;

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

Pix serp[64] =
{
  {3,3}
};

volatile int serpSize = 1;

int *serpX = &serp[0].x;
int *serpY = &serp[0].y;

int fruitX = 1;
int fruitY = 1;
int fruitLife = 0;

Pix stones[60];
int stonesN;


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

void eraseSnake(){
  matrix.drawPixel(serpX, serpY, 0);
}

void propagate() {
  int i;
  for (i = serpSize - 1; i > 0; i--) {
    serp[i].x = serp[i-1].x;
    serp[i].y = serp[i-1].y;
  }
}

int checkCollision() {
  int i;
  for (i = 1; i < serpSize; i++) {
    if (*serpX == serp[i].x && *serpY == serp[i].y)
      return -1;
  }
  for (i = 0; i < stonesN; i++) {
    if (*serpX == stones[i].x && *serpY == stones[i].y)
      return -1;
  }
  return 0;
}

int moveSnake() {
  propagate();
  if (dir == LEFT){
    *serpY -= 1;
    if (*serpY < 0) return -1;
  }
  else if (dir == RIGHT) {
    *serpY += 1;
    if (*serpY >= 8) return -1;
  }
  else if (dir == TOP) {
    *serpX += 1;
    if (*serpX >= 8) return -1;
  }
  else if (dir == BOTTOM) {
    *serpX -= 1;
    if (*serpX < 0) return -1;
  }
  return checkCollision();
}

void gameOver() {
  int i, j;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      matrix.drawPixel(i, j, colors[0]);
    }
  }
}

void showSnake(uint16_t color) {
  int i;
  for (i = 0; i < serpSize; i++) {
    if (serp[i].x != -1) // should never happen
      matrix.drawPixel(serp[i].x, serp[i].y, color);
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

void showFruit() {
  matrix.drawPixel(fruitX, fruitY, colors[4]);
}

void growSerp() {
  serp[serpSize].x = serp[serpSize - 1].x;
  serp[serpSize].y = serp[serpSize - 1].y;
  serpSize += 1;
}

void stonifyFruit() {
  stones[stonesN].x =  fruitX;
  stones[stonesN].y =  fruitY;
  newFruit();
  stonesN += 1;
  showStones();
}

void showStones() {
  int i;
  for (i = 0; i < stonesN; i++) {
    matrix.drawPixel(stones[i].x, stones[i].y, colors[0]);
  }
}

void newFruit() {
  fruitX = (fruitY + 1999) % 8;
  fruitY = (fruitX + 1999) % 8;
  fruitLife = 0;
}

void checkFruit(){
  if (*serpX == fruitX && *serpY == fruitY) {
    // new fruit
    newFruit();
    growSerp();
  }
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
    serpSize = 1;
    *serpX = *serpY = 3;
    showSnake(colors[1]);
    stonesN = 0;
    newFruit();
    showFruit();
    matrix.show();
    state = Normal;
  }
  else {
    showSnake(0); //erase snake
    int res = moveSnake();
    if (res == -1) {
      state = GameOver;
      gameOver();
    }
    else {
      checkFruit();
      showFruit();
      showSnake(colors[1]);
    }
    if (fruitLife == 15) {
      stonifyFruit();
    }
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
  fruitLife += 1;
}

void loop4(){
  alreadyPressed = 0;
  delay(200);
}


