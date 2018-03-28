#include <Adafruit_NeoPixel.h>

#define DATA 6
#define STRIPSIZE 50
#define ANALOG 7
#define COEFF (1023/STRIPSIZE)

void pause(int freq){
	delay(freq);
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIPSIZE, DATA, NEO_GRB + NEO_KHZ800);

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

const uint32_t colors[] = {
  strip.Color(255, 0, 0),
  strip.Color(0, 255, 0),
  strip.Color(0, 0, 255),
  strip.Color(255, 255, 0),
  strip.Color(0, 255, 255),
  strip.Color(255, 0, 255),
  strip.Color(0, 0, 0),
  strip.Color(255,255,255)
};

void unicolor(int color) {
	int i;
	for (i = 0; i < STRIPSIZE; i++) {
		strip.setPixelColor(i, colors[color]);
	}
}

void printChenillard(int size, int color, int leftBit) {
	int i;
	for (i = 0; i < size; i++) {
		strip.setPixelColor(i + leftBit, colors[color]);
	}
}

void degrade(int r, int g, int b) {
	int val = analogRead(ANALOG);
	int max = val/COEFF; 
	int i;
	for (i = 1; i < max; i++) {
		strip.setPixelColor(i, r/(max/i), g/(max/i), b/(max/i));
	}
	strip.show();
	pause(200);
	unicolor(NONE);
}

void chenillard(int size, int color, int freq) {
	while (true) {
		int i;
		for (i = 0; i + size < STRIPSIZE; i++) {
			printChenillard(size, color, i);
			strip.show();
			pause(freq);
			unicolor(NONE);
		}
		for (i = STRIPSIZE - size; i >= 0; i--) {
			printChenillard(size, color, i);
			strip.show();
			pause(freq);
			unicolor(NONE);
		}
	}
}

void potentioRead() {
	int val = analogRead(ANALOG);
	printChenillard(val / COEFF, CYAN, 0);
	strip.show();
	pause(10);
	unicolor(NONE);
}

void setup() {
	strip.begin();
	strip.show();
	Serial.begin(9600);
}

void loop(){
	int i, j, k;
	for (i = 0; i < 256; i+= 10)
		degrade(i, 255 - i, 255);
	for (i = 0; i < 256; i+= 10)
		degrade(255, i, 255 - i);
	for (i = 0; i < 256; i+= 10)
		degrade(255 - i, 255, i);
}