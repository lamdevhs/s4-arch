int red = 9;
int button = 8;
int buttonLastState = LOW;
int redState = LOW;
int val;

void toggle(int val){
  if (val == LOW) return HIGH;
  return LOW;
}

void setup(){
  pinMode(red, OUTPUT);
  pinMode(button, INPUT_PULLUP);
}

void loop(){
  val = digitalRead(button);
  if (val != buttonLastState && val == HIGH) {
    redState = toggle(redState);
    digitalWrite(red, redState);
    buttonLastState = val;
    delay(300);
  }
}