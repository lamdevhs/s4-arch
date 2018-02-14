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
  matrix.Color(255, 255, 0), matrix.Color(0, 255, 255), matrix.Color(255, 0, 255),};

int pos = 0;

void setup() {
  matrix.begin();
  matrix.setBrightness(10);
  //matrix.drawPixel(1,1,colors[0]);
  //matrix.drawPixel(1,4,colors[1]);
  //matrix.drawPixel(5,1,colors[2]);
  matrix.show();
}





void loop1() {
  matrix.drawPixel(2, pos % 8, colors[2]);
  matrix.drawPixel(pos % 8, 3, colors[0]);
  matrix.show();
  delay(500);
  matrix.drawPixel(2, pos % 8, 0);
  matrix.drawPixel(pos % 8, 3, 0);
  pos++;
}


void square(uint16_t color, int n) {
  n = n % 4;
  int inf = n;
  int sup = 7 - n;

  int i, j;
  for(i = inf; i<=sup; i++) {
    matrix.drawPixel(i, inf, color);
    matrix.drawPixel(inf, i, color);
    matrix.drawPixel(i, sup, color);
    matrix.drawPixel(sup, i, color);
  }
  matrix.show();
  delay(500);
  for(i = inf; i<=sup; i++) {
    matrix.drawPixel(i, inf, 0);
    matrix.drawPixel(inf, i, 0);
    matrix.drawPixel(sup, i, 0);
    matrix.drawPixel(i, sup, 0);
  }
}

void loop2() {
  square(colors[pos % 6],pos);
  pos++;
}

void loop() {
  
}


