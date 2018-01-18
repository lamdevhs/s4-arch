struct color {
  int red;
  int green;
  int blue;
};
typedef struct color color;

color fromRGB(int r, int g, int b){
  color c = {r, g, b};
}


// program data
#define N 8
long colors[N];
colors[0] = fromRGB(255, 0, 0);
colors[1] = fromRGB(0, 255, 0);
colors[2] = fromRGB(0, 0, 255);
colors[3] = fromRGB(0, 255, 255);
colors[4] = fromRGB(255, 0, 255);
colors[5] = fromRGB(255, 255, 0);
colors[6] = fromRGB(255, 255, 255);
colors[7] = fromRGB(0, 0, 0);

int freq = 1;
int colorIx = 0; // index in `colors`


// physical data
int red = 3;
int green = 5;
int blue = 6;
// DONT FORGET THE OHMS!!!

int setColor(color c){
  analogWrite(red, c.red);
  analogWrite(green, c.green);
  analogWrite(blue, c.blue);
}

void nextColor(){
  colorIx = (colorIx + 1) % N;
  setColor(colors[colorIx]);
  delay(1000/freq);
}

void setup(){
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
}

void loop(){
  nextColor();
}