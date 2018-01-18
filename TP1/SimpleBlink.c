int red = 2;

void setup(){
  pinMode(red, OUTPUT);
}

void blink(int what, int speed){
  int d = 1000/speed;
  digitalWrite(what, HIGH); delay(d);
  digitalWrite(red, LOW); delay(d);
}

void loop(){
  blink(red, 1);
}