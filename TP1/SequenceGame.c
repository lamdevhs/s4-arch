#include <time.h>
#include <stdlib.h>


// physical data
int yellow = 0; // ?????????
int red = 1;

int bYellow = 2;
int bRed = 3;

int iYellow = 0;
int iRed = 1;
  // ^ corresponding interrupters

int good = 5; // ????????????
int bad = 6;
  // analogy outputs for green/red light

// sequence
#define N 4
volatile int seq[N];
volatile int attemptIx = 0; // always will be % N


// state managers:
volatile bool attempting = false; // after showing sequence
volatile bool restart = false; // end game





//:::::::::::::::::::::::


int randColorSeq(){
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

void newSeq(){
  int i;
  for(i = 0; i < N; i++){
    seq[i] = randColorSeq();
  }
}

#define BLINK_N 10
// ^ how much the diod blinks when losing/winning
#define BLINK_RATE 300

void showSeq(){
  int i, which;
  for(i = 0; i < N; i++){
    which = seq[i];
    on(which); delay(1000);
    off(which); delay(BLINK_RATE);
      // ^ to make the change more obvious
  }
}


void blink(int which, int amount){
  int i;
  for (i = 0; i < amount; i++){
    digitalWrite(which, HIGH);
    delay(BLINK_RATE);
    digitalWrite(which, LOW);
    delay(BLINK_RATE);
  }
}

void ok(){
  blink(good, 1);
}

void youWon(){
  blink(good, BLINK_N);
}
void gameOver(){
  blink(bad, BLINK_N);
}



void onButton(int which){
  blink(which, 3); return;
  if (!attempting) return;
    // ^ not the right time, so do nothing
  if (seq[attemptIx] == which) {
    ok();
    attemptIx = (attemptIx + 1) % N;
    if (attemptIx == 0) { // successful end of attempt
      attempting = false;
      youWon();
      restart = true;  // game restarted
    }
  }
  else {
    attempting = false; // unsuccessful end of attempt
    gameOver();
    restart = true; // game restarted
  }
}

void onRedButton() {
  //digitalWrite(red, HIGH);
  onButton(red);
}
void onYellowButton() {
  //digitalWrite(yellow, HIGH);
  onButton(yellow);
}

//:::::::::::::::::::::

void setup(){
  srand(time(NULL));
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(bad, OUTPUT);
  pinMode(good, OUTPUT);
  pinMode(bYellow, INPUT);
  //pinMode(bRed, INPUT_PULLUP);
  attachInterrupt(iYellow, onYellowButton, RISING);
  attachInterrupt(iRed, onRedButton, RISING);
  //Serial.begin(9600);
  //digitalWrite(red, LOW);
}

void loop(){
  restart = false;
  newSeq();
  showSeq();
  attempting = true;
  while (!restart) {//blink(good, 5);
    }
    // ^ stuck till the attempt ends
    // either via youWon or gameOver
}
