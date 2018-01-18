#include <time.h>
#include <stdlib.h>

// physical data
int yellow = 0; // ?????????
int red = 1;

// sequence
#define N 4
int seq[N];
int attempt[N];
srand(time(NULL));

void randColor(){
  if (rand() % 2 == 0) {
    return yellow;
  }
  return red;
}

void off(int which){
  digitalWrite(which, LOW);
}
void on(int which){
  digitalWrite(which, HIGH);
}

void light(int which, int bulb){
  if (bulb == which){
    on(bulb);
  }
  else {
    off(bulb);
  }
}


void newSeq(){
  int i;
  for(i = 0; i < N; i++){
    seq[i] = randColor;
  }
}

void playSeq(){
  int i;
  for(i = 0; i < N; i++){
    int which = seq[i];
    on(which);
    delay(1000);
    off(which);
    delay(100); // to show the switching
  }
}

void setup(){
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  
}

void recordAttempt(){
  
}

void loop(){
  newSeq();
  playSeq();
  recordAttempt();
}