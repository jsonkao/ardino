// include the library header
// no font headers have to be included
#include <openGLCD.h>
#include "pitches.h"

// TODO: make different noises with jump/slide

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
const int startScore = 30;
unsigned long startMillis;
boolean over;
void setup()
{
  Serial.begin(9600); 
  GLCD.Init();
  GLCD.SelectFont(System5x7);

  drawSkeleton();
  drawScore(startScore);  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);
  startGame();
  GLCD.DrawString("Press any button...  Left = Jump, Right = Crouch", gTextfmt_left, gTextfmt_center);
  over = true;
}

int dinoSize = 6;
void drawSkeleton() {
  GLCD.FillRect(GLCD.Left, ground, GLCD.Right, GLCD.Bottom);
  drawDino(ground - dinoSize);
}

int blocks[startScore][4];
long placeBlockAt = 200;
int front;
int numBlocks;
void placeBlock() {
  boolean shouldDuck = random(3) < 1;
  int height = random(dinoSize*2, dinoSize*2 + (shouldDuck ? 2 : 6));
  int width = random(6, 8);
  
  // blocks are stored as { x, y, width, height }
  int i = front + numBlocks++;
  blocks[i][0] = GLCD.Width;
  blocks[i][2] = width;
  blocks[i][1] = ground - height;
  blocks[i][3] = shouldDuck ? dinoSize/2 : height;
}

void drawScore(int score) {
  GLCD.EraseTextLine(0);
  GLCD.PrintNumber(score);
}

void updateBlocks() {
  int bound = front + numBlocks;
  for (int i = front; i < bound; i++) { // logic needa be changed
    blocks[i][0] -= 2;
    int x = blocks[i][0];
    int y = blocks[i][1];
    int width = blocks[i][2];
    int height = blocks[i][3];
    GLCD.FillRect(x + width, y, 2, height, PIXEL_OFF); // undraw tail
    if (x > GLCD.Left) {
      GLCD.FillRect(x, y, 2, height, PIXEL_ON); // redraw head
    } else if (x + width <= GLCD.Left) {
      front++;
      drawScore(startScore - front);
      numBlocks--;
    }
  }
}

const int dinoX = GLCD.Left + dinoSize + 6;
int dinoY = 0;
boolean isCrouching;
void drawDino(int y) {
  if (isCrouching) {
    dinoY = y;
    GLCD.DrawCircle(dinoX, dinoY - round(dinoSize / 2), dinoSize, PIXEL_OFF);
    GLCD.FillEllipse(dinoX, dinoY, dinoSize, round(dinoSize/2));
  } else {
    GLCD.DrawCircle(dinoX, dinoY, dinoSize, PIXEL_OFF);
    GLCD.FillEllipse(dinoX, dinoY + round(dinoSize / 2), dinoSize, round(dinoSize/2), PIXEL_OFF); // undraw ellipse
    dinoY = y;
    GLCD.DrawCircle(dinoX, dinoY, dinoSize);
  }
}
void drawDino() {
  drawDino(dinoY);
}

const double jumpV = -5.0;
boolean goingUp;
double dinoV;
const double g = 0.6;
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

boolean isJumping;
boolean saved;
unsigned long currentMillis;
const unsigned long period = 50;
void startGame() {
  isJumping = false;
  saved = false;
  over = false;
  goingUp = true;
  isCrouching = false;
  dinoV = 0.0;
  front = 0;
  numBlocks = 0;
  dinoY = ground - dinoSize;
  drawScore(startScore);
  GLCD.DrawString("                                                            ", gTextfmt_center, gTextfmt_center, eraseFULL_LINE);
  startMillis = millis();
  placeBlockAt = startMillis + 200;
}

void loop() {
  currentMillis = millis();
  if (currentMillis - startMillis < period) {
    return;
  }
  if (over) {
    if (digitalRead(buttonApin) == LOW || digitalRead(buttonBpin) == LOW) {
      startGame();
    }
    return;
  }
  if (front == startScore) {
    GLCD.DrawString("YOU WON!!", gTextfmt_center, gTextfmt_row(2), eraseFULL_LINE);
    GLCD.DrawString("Press a button to restart.", gTextfmt_center, gTextfmt_center);
    if (digitalRead(buttonApin) == LOW || digitalRead(buttonBpin) == LOW) {
      startGame();
    }
    return;
  }

  if (millis() >= placeBlockAt) {
    placeBlock();
    placeBlockAt += random(3500, 5000);
  }
  updateBlocks();
  if (digitalRead(buttonApin) == LOW) { // jump
    if (!isJumping && !isCrouching) {
      isJumping = true;
      dinoV = jumpV;
    }
  }
  if (!isJumping) {
    if (digitalRead(buttonBpin) == LOW && !isCrouching) {
      isCrouching = true;
      dinoY += round(dinoSize / 2);
    }
    if (digitalRead(buttonBpin) == HIGH && isCrouching) {
      isCrouching = false;
      dinoY -= round(dinoSize / 2);
    }
  }
  if (isJumping) {
    isJumping = jump();
  } else {    
    drawDino();
  }
  if (collided()) {
    GLCD.DrawString("Game over. Press any button to restart.", gTextfmt_center, gTextfmt_center);  
    over = true;
  }
  startMillis = currentMillis;
}

boolean collided() {
  if (numBlocks == 0) {
    return false;
  }
  int e = 1;
  int boxR = dinoSize - e;
  int x = dinoX - boxR;
  int y = dinoY - boxR;
  int w = 2*boxR;
  int h = 2*boxR;
  int bx = blocks[front][0];
  int by = blocks[front][1];
  int bu = bx + blocks[front][2];
  int bv = by + blocks[front][3];
  for (int haveW = 0; haveW < 2; haveW++) {
    for (int haveH = 0; haveH < 2; haveH++) {
      int px = x + haveW * w;
      int py = y + haveH * h;
      if (px >= bx && px <= bu && py >= by && py <= bv) {
        return true;
      }
    }
  }
  return false;
}
 
