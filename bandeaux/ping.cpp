#include <Adafruit_NeoPixel.h>

#define DATA 6
#define BT4 3
#define BT2 2
#define BT3 18
#define BT1 19

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
	pause(100);
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


// -------------------------
// -------------------------
// -------------------------

#define MINfREQ 20

volatile int volatileSize = 5;
volatile int volatileFreq = 5;
volatile int volatileStopped = 0;
volatile int paceMaker = 0;

void chenillard2(int color) {
	while (true) {
		int i;
		for (i = 0; i + volatileSize < STRIPSIZE; i++) {
			printChenillard(volatileSize, color, i);
			strip.show();
			paceMaker = 0;
			pause(volatileFreq*MINfREQ);
			unicolor(NONE);
			potentioReadSize();
			if (volatileStopped) i--;
			paceMaker = 0;

			Serial.println(volatileFreq);
		}
		for (i = STRIPSIZE - volatileSize; i >= 0; i--) {
			printChenillard(volatileSize, color, i);
			strip.show();
			paceMaker = 0;
			pause(volatileFreq*MINfREQ);
			unicolor(NONE);
			potentioReadSize();
			if (volatileStopped) i++;
			paceMaker = 0;

			Serial.println(volatileFreq);
		}
	}
}

void iBT1() {
	if (paceMaker) return;
	volatileStopped = !volatileStopped;
	paceMaker = 1;
}



void iSpeed(int k){
	if (paceMaker) return;
	volatileFreq = max(volatileFreq + k, 1);
	paceMaker = 1;
}

void iBT3() {
	iSpeed(-1);
}
void iBT4() {
	iSpeed(1);
}


void potentioReadSize() {
	int val = analogRead(ANALOG);
	volatileSize = max(1, min(val / COEFF, STRIPSIZE / 2));
}



//-----------------
//-----------------
//-----------------
//-----------------

void printField(int color){
	//unicolor(NONE);
	strip.setPixelColor(8, colors[color]);
	strip.setPixelColor(42, colors[color]);
}

void setup() {
	strip.begin();
	strip.show();
	pinMode(BT1, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BT1), iBT1, FALLING);
	pinMode(BT3, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BT3), iBT3, FALLING);
	pinMode(BT4, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(BT4), iBT4, FALLING);
	Serial.begin(9600);
}

void pingPong(int size, int color){
	int i;
	for (i = 0; i + size < STRIPSIZE; i++) {
		printChenillard(size, color, i);
		printField(WHITE);
		strip.show();
		paceMaker = 0;
		pause(volatileFreq*MINfREQ);
		unicolor(NONE);
		potentioReadSize();
		if (volatileStopped) i--;
		paceMaker = 0;

		Serial.println(volatileFreq);
	}
	for (i = STRIPSIZE - size; i >= 0; i--) {
		printChenillard(size, color, i);
		printField(WHITE);
		strip.show();
		paceMaker = 0;
		pause(volatileFreq*MINfREQ);
		unicolor(NONE);
		potentioReadSize();
		if (volatileStopped) i++;
		paceMaker = 0;

		Serial.println(volatileFreq);
	}
}

void loop(){
	if (mode == Normal) {
		pingPong(4, RED);
	}
	else if (mode == GameOver) {
		
	}
}