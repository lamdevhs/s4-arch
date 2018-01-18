int red = 2;
int button = 2;
int buttonLastState = LOW;
int val;

void setup(){
  pinMode(red, OUTPUT);
  pinMode(button, INPUT);
  // ^ need R = 10 kOhm
}

void loop(){
  val = digitalRead(button);
  if (val != buttonLastState) {
    digitalWrite(red, val);
    buttonLastState = val;
  }
}