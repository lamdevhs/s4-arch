int red = 9;
int button = 2;
volatile int state = 0;
volatile bool change = false;

void toggle(int val){
  if (val == LOW) return HIGH;
  return LOW;
}

void stateVal(int val){
  if (val % 2 == 0) return LOW;
  return HIGH;
}

void pressingButton(){
  //state = 1 - state;
  state++;
  change = true;
}

void setup(){
  pinMode(red, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(0, pressingButton, RISING);
}

void loop(){
  if (change) {
    digitalWrite(red, stateVal(state));
    change = false;
  }
}