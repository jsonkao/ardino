// include the library header
// no font headers have to be included
#include <openGLCD.h>
#include "pitches.h"

int ledPin = 2;
int buttonApin = 12;
int buttonBpin = 13;

int soundPin = 3;
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
void soundIt() {
  // TODO: make different nouses when jump/slide
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(soundPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(soundPin);
  }
}
const int ground = GLCD.Bottom - 10;

unsigned long startMillis;
void setup()
{
  Serial.begin(9600); 
  GLCD.Init();
  GLCD.SelectFont(System5x7);

  drawSkeleton();

  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);
  startMillis = millis();
}

int dinoSize = 6;
void drawSkeleton() {
  GLCD.FillRect(GLCD.Left, ground, GLCD.Right, GLCD.Bottom);
  drawDino(ground - dinoSize);
}

int blocks[100][3];
long placeBlockAt = 200;
int front = 0;
int numBlocks = 0;
void placeBlock() {
  int height = random(10, 14);
  int width = random(6, 8);
  // blocks are stored as { x, y, width }
  int i = front + numBlocks++;
  blocks[i][0] = GLCD.Width;
  blocks[i][1] = ground - height;
  blocks[i][2] = width;
}

void updateBlocks() {
  int bound = front + numBlocks;
  for (int i = front; i < bound; i++) { // logic needa be changed
    blocks[i][0] -= 2;
    int x = blocks[i][0];
    int y = blocks[i][1];
    int width = blocks[i][2];
    int height = ground - y;
    GLCD.FillRect(x + width, y, 2, height, PIXEL_OFF); // undraw tail
    if (x > GLCD.Left) {
      GLCD.FillRect(x, y, 2, height, PIXEL_ON); // redraw head
    } else if (x + width <= GLCD.Left) {
      front++;
      numBlocks--;
    }
  }
}

int dinoX = GLCD.Left + dinoSize + 6;
int dinoY = 0;
void drawDino(int y) {
  GLCD.DrawCircle(dinoX, dinoY, dinoSize, PIXEL_OFF);
  dinoY = y;
  GLCD.DrawCircle(dinoX, dinoY, dinoSize);
}
void drawDino() {
  drawDino(dinoY);
}

double jumpV = -5.0;
boolean goingUp = true;
double dinoV = 0.0;
double g = 0.6;
boolean jump() {
  int newY = dinoY + round(dinoV);
  if (newY + dinoSize < ground) {
    drawDino(newY);
    dinoV += g;
    return true;
  } else {
    drawDino(ground - dinoSize);
    return false;
  }
}

boolean isJumping = false;
unsigned long currentMillis;
const unsigned long period = 50;
void loop() {
  currentMillis = millis();
  if (currentMillis - startMillis < period) {
    return;
  }
  // Serial.println(currentMillis);
  if (millis() >= placeBlockAt) {
    placeBlock();
    placeBlockAt += random(3500, 5000);
  }
  updateBlocks();
  if (digitalRead(buttonApin) == LOW) { // jump
    if (!isJumping) {
      isJumping = true;
      dinoV = jumpV;
    }
  }
  if (digitalRead(buttonBpin) == LOW) {
  }

  if (isJumping) {
    isJumping = jump();
  } else {    
    drawDino();
  }
  startMillis = currentMillis;
}
