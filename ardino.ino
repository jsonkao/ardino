// include the library header
// no font headers have to be included
#include <openGLCD.h>

const int ground = GLCD.Bottom - 10;

void setup() {
  Serial.begin(9600);
  // Initialize the GLCD 
  GLCD.Init();

 // Select the font for the default text area
  GLCD.SelectFont(System5x7);
  GLCD.print("hello, world!");

  drawSkeleton();
}

int dinoSize = 12;
int dinoX = 0;
int dinoY = 0;
void drawDino(int x, int y) {
  GLCD.FillCircle(dinoX, dinoY, dinoSize, PIXEL_OFF);
  GLCD.FillCircle(x, y, dinoSize);
}

void drawSkeleton() {
  GLCD.FillRect(GLCD.Left, ground, GLCD.Right, GLCD.Bottom);
  drawDino(GLCD.Left + dinoSize + 6, ground - dinoSize);
}

int blocks[10][3];
int numBlocks = 0;
long placeBlockAt = 200;

void placeBlock() {
  int height = random(14, 20);
  int width = random(8, 12);
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

int jumpHeight = 20;
boolean jumpDino() {
  if (dinoY < jumpHeight) {
    dinoY++;
  } else {
    dinoY--;
  }
  return dinoY >= ground; //VELEAADSBBOOLEANB
}

boolean isJumping = false;
void loop() {
  if (millis() >= placeBlockAt) {
    placeBlock();
    placeBlockAt += random(2500, 5000);
  }
  updateBlocks();
  if (!isJumping && random(0, 4000) == 1) { // jump randomly
    isJumping = true;
  }
  if (isJumping) {
    jumpDino();
    if (dinoY <= ground) {
      isJumping = false;
    }
  }
  delay(100);
}
