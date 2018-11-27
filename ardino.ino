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

void setup()
{
  Serial.begin(9600); 
  GLCD.Init();
  GLCD.SelectFont(System5x7);

  drawSkeleton();

  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);
}

int dinoSize = 12;
void drawSkeleton() {
  GLCD.FillRect(GLCD.Left, ground, GLCD.Right, GLCD.Bottom);
  drawDino(ground - dinoSize);
}

int blocks[10][3];
int numBlocks = 0;
long placeBlockAt = 200;

void placeBlock() {
  int height = random(4, 8);
  int width = random(4, 6);
  // blocks are stored as { x, y, width }
  blocks[numBlocks][0] = GLCD.Width;
  blocks[numBlocks][1] = ground - height;
  blocks[numBlocks][2] = width;
  numBlocks++;
}

void updateBlocks() {
  for (int i = 0; i < numBlocks; i++) {
    blocks[i][0]--;
    int x = blocks[i][0];
    int y = blocks[i][1];
    int width = blocks[i][2];
    int height = ground - y;
    GLCD.FillRect(x + width, y, 1, height, PIXEL_OFF); // undraw tail
    GLCD.FillRect(x, y, 1, height, PIXEL_ON); // redraw head
  }
}

int dinoX = GLCD.Left + dinoSize + 6;
int dinoY = 0;
void drawDino(int y) {
  GLCD.DrawCircle(dinoX, dinoY, dinoSize, PIXEL_OFF);
  GLCD.DrawCircle(dinoX, y, dinoSize);
}

int jumpHeight = 20;
boolean goingUp = false;
boolean jump() {
  if (goingUp) {
    drawDino(dinoY--);
    if (dinoY <= ground - jumpHeight) {
      goingUp = false;
    }
    return true;
  }
  drawDino(dinoY++);
  return dinoY >= ground;
}

boolean isJumping = false;
void loop() {
  if (millis() >= placeBlockAt) {
    placeBlock();
    placeBlockAt += random(3000, 4000);
  }
  updateBlocks();
  if (digitalRead(buttonApin) == LOW) { // jump
    if (!isJumping) isJumping = true;
    GLCD.print("mei's butt");
    soundIt();
  }
  if (digitalRead(buttonBpin) == LOW) {
    GLCD.print("is hairy");
    soundIt();
  }

  if (isJumping) {
    isJumping = jump();
  }
  delay(100);
}
