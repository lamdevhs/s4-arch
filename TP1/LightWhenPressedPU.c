int red = 2;
int button = 2;
int buttonLastState = LOW;
int val;

void setup(){
  pinMode(red, OUTPUT);
  pinMode(button, INPUT_PULLUP);
}

void loop(){
  val = digitalRead(button);
  if (val != buttonLastState) {
    digitalWrite(red, val);
    buttonLastState = val;
  }
}