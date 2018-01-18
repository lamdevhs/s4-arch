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

void setup(){
  pinMode(red, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop(){
  int val = digitalRead(button);
  if (val == HIGH) {
    state += 1;
    Serial.println(state);
    change = true;
  }
  if (change) {
    digitalWrite(red, stateVal(state));
    change = false;
  }
}