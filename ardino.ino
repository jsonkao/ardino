// include the library header
// no font headers have to be included
#include <openGLCD.h>

const int ground = GLCD.Bottom - 10;

void setup()
{
  Serial.begin(9600);
  // Initialize the GLCD 
  GLCD.Init();

 // Select the font for the default text area
  GLCD.SelectFont(System5x7);
  GLCD.print("hello, world!");

  drawSkeleton();
}

void drawSkeleton() {
  GLCD.FillRect(GLCD.Left, ground, GLCD.Right, GLCD.Bottom);    
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

void loop() {
  if (millis() >= placeBlockAt) {
    placeBlock();
    placeBlockAt += random(400, 600);
  }
  updateBlocks();
}
