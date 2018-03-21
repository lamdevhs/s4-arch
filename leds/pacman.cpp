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


int pacmanColor = BLUE;
int nothingColor = NONE;
int wallColor = RED;
int ghostColor = WHITE;
int fruitColor = GREEN;
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
#define MAXGHOSTS 6
#define MAXOBSTACLES MAXMAP*MAXMAP
#define MAXFRUITS MAXMAP*MAXMAP

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

typedef struct ghost {
  int x;
  int y;
  int dir;
} Ghost;

Ghost ghosts[MAXGHOSTS];
int nghosts = 0;
int nfruits = 0;

volatile int pacY = 0;
volatile int pacX = 0;

int PACX_REAL = 4;
int PACY_REAL = 4;


char strmap[] =
  "......../"
  ".C.....G"
  "..#.#.##"
  "#.#....."
  "....#..#"
  "#.#...#."
  "..#.#G.#"
  "#.....##";

// char strmap[] =
//   "#############/"
//   "#C..#...#..G#"
//   "#.#.#..##...#"
//   "#.#.#.....#.#"
//   "#...#..##.#.#"
//   "#.#..G.#.G..#"
//   "#.###...#.#.#"
//   "#......##...#"
//   "#############";

void buildMap() {
  int x, y;
  emptyMap();
  readMap(strmap);
}

void readMap(char *map) {
  nghosts = 0; //(re)initializes the variable
  int i;
  int line = 0;
  int col = 0;
  int pacmanAdded = 0;
  int linesize = -1;
  int lineSizeKnown = 0;
  for (i = 0; map[i] != '\0' && line < MAXMAP; i++, col++) {
    char c = map[i];
    int typeHere;
    if (col == linesize) {
      col = 0;
      line += 1;
    }
    if (c == '/') {
      if (lineSizeKnown) continue; //ignore it
      else {
        lineSizeKnown = 1;
        linesize = i;
        col = 0;
        line += 1;
        continue;
      }
    }
    switch (c) {
      case '#': typeHere = WALL; break;
      case 'G': typeHere = GHOST; break;
      case '.': typeHere = FRUIT; nfruits++; break;
      default: typeHere = NOTHING; break;
    }
    if (c == 'C' && !pacmanAdded) {
      addPacman(line, col); continue;
    }
    if (c == 'G' && nghosts < MAXGHOSTS) {
      addGhost(line, col); continue;
    }
    addPoint(line, col, typeHere);
  }
}

void emptyMap() {
  int x, y;
  for (x = 0; x < sizeMapX; x++) {
    for (y = 0; y < sizeMapY; y++) {
      addPoint(x, y, WALL);
    } 
  }
}

// void addFruits() {
//   int x, y;
//   for (x = 0; x < sizeMapX; x++) {
//     for (y = 0; y < sizeMapY; y++) {
//       if (gameMap[x][y] == NOTHING) {
//         addPoint(x, y, FRUIT);
//         nfruits++;
//       }
//     }
//   }
// }

// void addWalls() {
//   int x, y;
//   for (x = 0; x < sizeMapX; x++) {
//     for (y = 0; y < sizeMapY; y++) {
//       if (x % 2 == 1 && y % 2 == 1) {
//         addPoint(x, y, WALL);
//       }
//     } 
//   }
// }

void addPacman(int x, int y) {
  addPoint(x, y, PACMAN);
  pacX = x;
  pacY = y;
}

void addGhost(int x, int y) {
  if (nghosts >= MAXGHOSTS) {
    addPoint(x, y, NOTHING);
    return;
  }
  // else
  addPoint(x, y, GHOST);
  ghosts[nghosts].x = x;
  ghosts[nghosts].y = y;
  ghosts[nghosts].dir = nghosts % 4;
  nghosts++;
}

// void addWall(int xStart, int yStart, int xEnd, int yEnd) {
//   if (xStart != xEnd && yStart != yEnd) return; // wrong input
//   int isHorizontal = yStart == yEnd;
//   int length;
//   if (isHorizontal) {
//     if (xStart > xEnd) {
//       int tmp = xStart;
//       xStart = xEnd;
//       xEnd = tmp;
//     }
//     length = xEnd - xStart + 1;
//   }
//   else {
//     if (yStart > yEnd) {
//       int tmp = yStart;
//       yStart = yEnd;
//       yEnd = tmp;
//     }
//     length = yEnd - yStart + 1;
//   }
//   int i;
//   for (i = 0; i < length; i++) {
//     addPoint(
//       xStart + isHorizontal*i,
//       yStart + (1-isHorizontal)*i,
//       WALL);
//   }
// }

void addPoint(int x, int y, int type) {
  if (x < 0 || x >= MAXMAP
   || y < 0 || y >= MAXMAP) return;
  gameMap[x][y] = type;
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

void movePacman() {
  int tempX = pacX;
  int tempY = pacY;
  movePos(movdir, &tempX, &tempY);

  if (isMoveImpossible(tempX, tempY)) return;
    // we don't move coz we can't move

  addPoint(pacX, pacY, NOTHING);
  pacX = tempX;
  pacY = tempY;
  if (gameMap[pacX][pacY] == FRUIT) {
    nfruits -= 1;
  }
  addPoint(pacX, pacY, PACMAN);
  if (STATIC) {
    PACX_REAL = pacX;
    PACY_REAL = pacY;
  }
}

int moveGhosts() {
  int i;
  for (i = 0; i < nghosts; i++) {
    int theEnd = moveGhost(i);
    if (theEnd) return 1;
  }
  return 0;
}

int moveGhost(int g) {
  int dir = ghosts[g].dir;
  int x = ghosts[g].x;
  int y = ghosts[g].y;
  int newX, newY;
  int newdir; int n = 0;
  for (newdir = dir; n < 4; newdir = (newdir+1)%4) {
    if (n != 0 && newdir == dir) {
      continue;
    }
    newX = x; newY = y;
    movePos(newdir, &newX, &newY);
    if (newX == pacX && newY == pacY) {
      return 1; // game over
    }
    if (!isMoveImpossible(newX, newY)) {
      ghosts[g].dir = newdir;
      break;
    }
    if (n == 0) {
      newdir = rand();
    }
    n++;
  }
  if (n == 4) { // we failed to find a proper direction
      return 0; // we return without moving
  }
  // else

  if (gameMap[newX][newY] == FRUIT) {
    addPoint(newX, newY, GHOST_FRUIT);
  }
  else {
    addPoint(newX, newY, GHOST);
  }

  if (gameMap[x][y] == GHOST_FRUIT) {
    addPoint(x, y, FRUIT);
  }
  else {
    addPoint(x, y, NOTHING);
  }
  ghosts[g].x = newX;
  ghosts[g].y = newY;
  return 0;
}

int colorizeMap(int type) {
  switch(type) {
    case FRUIT: return fruitColor;
    case GHOST:
    case GHOST_FRUIT: return ghostColor;
    case PACMAN: return pacmanColor;
    case NOTHING: return nothingColor;
    case WALL: return wallColor;
    default: return bugColor;
  }
}

void showAll() {
  int x, y;
  for (x = 0; x < MATX; x++) {
    for (y = 0; y < MATY; y++) {
      int virtX = virtPosX(x);
      int virtY = virtPosY(y);
      int color;
      if (virtX == -1 || virtY == -1) {
        color = wallColor;
      }
      else {
        int type = gameMap[virtX][virtY];
        color = colorizeMap(type);
      }
      matrix.drawPixel(x, y,
        colors[color]);
    }
  }
  // matrix.drawPixel(PACX_REAL, PACY_REAL,
  //   colors[pacmanColor]);
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

int virtPosX(int realX){
  int val = realX + pacX - PACX_REAL;
  if (val < 0 || val >= MAXMAP) return -1;
  else return val;
}
int virtPosY(int realY){
  int val = realY + pacY - PACY_REAL;
  if (val < 0 || val >= MAXMAP) return -1;
  else return val;
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
  if (state == GameOver) {}
  else if (state == Init) {
    unicolor(colors[NONE]);
    buildMap();
    showAll();
    matrix.show();
    movdir = Right;
    state = Normal;

  }
  else { // Normal state
    unicolor(colors[NONE]);
    movePacman();
    int theEnd = moveGhosts();
    showAll();
    if (theEnd) {
      gameOver(LOST);
      state = GameOver;
    }
    if (nfruits == 0) {
      gameOver(WON);
      state = GameOver;
    }
  }
  matrix.show();
  alreadyPressed = 0;
  delay(FREQ);
}


