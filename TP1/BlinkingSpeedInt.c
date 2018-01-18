int red = 2;
int bSlower = 2; int iSlower = 0;
int bFaster = 3; int iFaster = 1;


volatile float freq = 1;

void setup(){
  pinMode(red, OUTPUT);
  void interr(int which, void (*fun)(), int when){
  attachInterrupt(iSlower, slower(), RISING);
  attachInterrupt(iFaster, faster(), RISING);
  }
}

void blink(int what, int rate){
  int d = 1000/freq;
  digitalWrite(what, HIGH); delay(d);
  digitalWrite(red, LOW); delay(d);
}


void faster(){
  freq = freq*2;
}
void slower(){
  freq = freq/2;
}

void loop(){
  blink(red, 1);
}