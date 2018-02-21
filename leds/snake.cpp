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

int pinClockwise = 2;
int pinCounterClockwise = 3;
int donnees= 6;

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

int fruitColor = BLUE;
int snakeColor = GREEN;
int stoneColor = RED;


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

Pix snake[64];
// {
//   {3,3}
// };

volatile int snakeSize = 1;

// head of the snake
int *snakeX = &snake[0].x;
int *snakeY = &snake[0].y;

// fruit
Pix fruit; // = {1,1};
int fruitAge = 0;
  // age of the fruit, in time units
#define ROTTEN 15
  // ^ fruits get rotten/stonified when they reach 15

// fruits that became stones after they died
Pix stones[64];
int stonesN; // number of stones


// general states of the game:
// Init = before the game (re)starts, to reset all the variables properly
enum States {
  Init, GameOver, Normal
};

enum DIR {
  LEFT,
  TOP,
  RIGHT,
  BOTTOM
};

volatile int state = Init;
volatile int snakeDir = RIGHT;

volatile int alreadyPressed = 0;
  // prevents a button from being pressed more than once
  // during one time unit of the game

#define FREQ 500
  // frequency of the game
  // aka time unit of the game, in ms

// move the snake by propagating the coordinates
// of each pixel to the previous one, starting from
// the tail, ending before the head
void propagate() {
  int i;
  for (i = snakeSize - 1; i > 0; i--) {
    snake[i].x = snake[i-1].x;
    snake[i].y = snake[i-1].y;
  }
}

// check collisions of the snake's head
// with the pixels of its own body, and with
// the stones (rotten fruits)
// returns 1 on collision detected
int checkCollision() {
  int i;
  for (i = 1; i < snakeSize; i++) {
    if (*snakeX == snake[i].x && *snakeY == snake[i].y)
      return 1;
  }
  for (i = 0; i < stonesN; i++) {
    if (*snakeX == stones[i].x && *snakeY == stones[i].y)
      return -1;
  }
  return 0;
}

// move the snake along its current direction
// returns 1 if a collision with a wall, itself,
// or a stone occurred and 0 otherwise
int moveSnake() {
  propagate(); // move body except head

  // move head:
  if (snakeDir == LEFT){
    *snakeY -= 1;
    if (*snakeY < 0) return 1;
  }
  else if (snakeDir == RIGHT) {
    *snakeY += 1;
    if (*snakeY >= MATY) return 1;
  }
  else if (snakeDir == TOP) {
    *snakeX += 1;
    if (*snakeX >= MATX) return 1;
  }
  else if (snakeDir == BOTTOM) {
    *snakeX -= 1;
    if (*snakeX < 0) return 1;
  }
  return checkCollision();
}

// prints a very angry, very red matrix
// of very angry red leds
void gameOver() {
  unicolor(colors[RED]);
}

void showSnake(int erase) {
  int i;
  for (i = 0; i < snakeSize; i++) {
    matrix.drawPixel(snake[i].x, snake[i].y,
      colors[erase ? NONE : snakeColor]);
  }
}

void btnClockwise(){
  if (alreadyPressed) return;
  if (state == GameOver)
    state = Init; // game over + button -> restart
  else snakeDir = (snakeDir + 1) % 4;
    // ^ change direction clockwise
  alreadyPressed = 1;
}

void btnCounterClockwise(){
  if (alreadyPressed) return;
  if (state == GameOver)
    state = Init; // game over + button -> restart
  else snakeDir = (snakeDir + 3) % 4;
    // ^ change direction counter-clockwise
  alreadyPressed = 1;
}

void showFruit() {
  matrix.drawPixel(fruit.x, fruit.y, colors[fruitColor]);
}

// add a new pixel to the snake, originally superposed
// to its previously-last pixel, till the next propagate()
void growSnake() {
  snake[snakeSize].x = snake[snakeSize - 1].x;
  snake[snakeSize].y = snake[snakeSize - 1].y;
  snakeSize += 1;
}

// create a stone from current fruit,
// create new fruit to replace it
// update printing the stones
void stonifyFruit() {
  stones[stonesN].x =  fruit.x;
  stones[stonesN].y =  fruit.y;
  newFruit();
  stonesN += 1;
  showStones();
}

void showStones() {
  int i;
  for (i = 0; i < stonesN; i++) {
    matrix.drawPixel(stones[i].x, stones[i].y, colors[stoneColor]);
  }
}

// creates a randomly-placed fruit
// avoids the problem of creating a fruit
// right under a stone or under the snake's body
// which would end with squashed fruits and all that would
// be messy...
void newFruit() {
  fruitAge = 0;
  int goodFruit = 0;
  while (!goodFruit) {
    goodFruit = 1; // we start optimistic
    fruit.x = rand() % MATX;
    fruit.y = rand() % MATY;
    int i;
    
    for (i = 0; i < snakeSize; i++) {
      if (fruit.x == snake[i].x && fruit.y == snake[i].y) {
        goodFruit = 0; break;
      }
    }
    if (!goodFruit) continue; // next while loop

    for (i = 0; i < stonesN; i++) {
      if (fruit.x == stones[i].x && fruit.y == stones[i].y) {
        goodFruit = 0; break;
      }
    }
  }
}

// check whether the snake is eating the fruit
// deal with the consequences
void checkFruit(){
  if (*snakeX == fruit.x && *snakeY == fruit.y) {
    // new fruit
    newFruit();
    growSnake();
  }
}

void setup() {
  srand(time(NULL));
  pinMode(pinClockwise, INPUT_PULLUP);
  pinMode(pinCounterClockwise, INPUT_PULLUP);
  attachInterrupt(0, btnClockwise, FALLING);
  attachInterrupt(1, btnCounterClockwise, FALLING);
  matrix.begin();
  matrix.setBrightness(10);

}

void loop() {
  if (state == GameOver)
    gameOver();
  else if (state == Init) {
    unicolor(colors[NONE]);
    snakeSize = 1;
    *snakeX = *snakeY = 3;
    showSnake(0);
    stonesN = 0;
    newFruit();
    showFruit();
    matrix.show();
    state = Normal;
  }
  else {
    showSnake(1); // 1 = erase snake
    int collision = moveSnake();
    if (collision) {
      state = GameOver;
      gameOver();
    }
    else {
      checkFruit();
      showFruit();
      showSnake(0);
    }
    if (fruitAge == ROTTEN) { // fruit life is rotten
      stonifyFruit();
    }
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
  fruitAge += 1;
}


