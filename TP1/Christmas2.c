#define N 8
#define S 500
int colors[24] = {
  0,255,0,
  255,0,0,
  0,0,255,
  255,255,0,
  255,0,255,
  0,255,255,
  255,255,255,
  0,0,0
};

void setColor(int ix){
  analogWrite(red, colors[ix*3]);
  analogWrite(green, colors[ix*3 + 1]);
  analogWrite(blue, colors[ix*3 + 2]);
}

int red = 3, green = 5, blue = 6;

void setup(){
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
}

void loop(){
  for (int i = 0; i < 8; i++) {
    setColor(i);
    delay(S);
  }
}
