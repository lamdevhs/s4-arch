#include <time.h>
#include <stdlib.h>


// physical data
int yellow = 0; // ?????????
int red = 1;

int iYellow = 2;
int iRed = 3;
  // ^ corresponding interrupters

int good = 5; // ????????????
int bad = 6;
  // analogy outputs for green/red light

// sequence
#define N 4
int seq[N];
int attemptIx = 0; // always will be % N


// state managers:
bool attempting = false; // after showing sequence
bool restart = false; // end game





//:::::::::::::::::::::::

srand(time(NULL));
void randColorSeq(){
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
  int i;
  for(i = 0; i < N; i++){
    int which = seq[i];
    on(which); delay(1000);
    off(which); delay(BLINK_RATE);
      // ^ to make the change more obvious
  }
}

void ok(){
  blinkColor(good, 1);
}

void youWon(){
  blink(good, BLINK_N);
}
void gameOver(){
  blinkColor(bad, BLINK_N);
}

void blink(int which, int amount){
  int i;
  for (i = 0; i < amount; i++){
    analogWrite(which, 255);
    delay(BLINK_RATE);
  }
}


void onButton(int which){
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


void onRedButton() {onButton(red)};
void onYellowButton() {onButton(yellow)};

//:::::::::::::::::::::

void setup(){
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(bad, OUTPUT);
  pinMode(good, OUTPUT);
  attachInterrupt(iYellow, onYellowButton, RISING);
  attachInterrupt(iRed, onRedButton, RISING);
}

void loop(){
  restart = false;
  newSeq();
  showSeq();
  attempting = true;
  while (!restart) {}
    // ^ stuck till the attempt ends
    // either via youWon or gameOver
}