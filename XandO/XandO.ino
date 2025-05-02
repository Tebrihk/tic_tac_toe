#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#include <Adafruit_GFX.h>

// Create TFT object
MCUFRIEND_kbv tft;

// TouchScreen pins
#define YP A2
#define XM A3
#define YM 8
#define XP 9

// TouchScreen calibration values (adjust for your screen)
#define TS_MINX 940
#define TS_MAXX 84
#define TS_MINY 131
#define TS_MAXY 904

// Initialize touchscreen with resistance value (300 ohms typical)
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Define minimum and maximum valid touch pressure
#define MINPRESSURE 1
#define MAXPRESSURE 1000

// Colors
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F

// Game variables
char board[3][3];  // Game board state (3x3)

bool playerX = true;  // true = X, false = O


void setup() {
  Serial.begin(9600);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  drawGrid();
  resetBoard();
}

void loop() {
  TSPoint p = ts.getPoint(); // Get touch point

  // Restore pins for display use after touch reading
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {         // If touch is valid
    int x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width()); // Convert X coord
    int y = map(p.x, TS_MINX, TS_MAXX, 0, tft.height()); // Convert Y coord

    int col = x / 80;  // Convert pixel X to column (0–2)
    int row = y / 80;   // Convert pixel Y to row (0–2)

    // If within grid and the cell is empty
    if (row < 3 && col < 3 && board[row][col] == ' ') {
      board[row][col] = playerX ? 'X' : 'O';  // Place symbol
      drawSymbol(row, col, board[row][col]);   // Draw symbol on screen
      if (checkWinner()) {
        tft.fillScreen(BLACK);
        tft.setTextColor(WHITE);
        tft.setTextSize(3);
        tft.setCursor(60, 100);
        tft.print("Player ");
        tft.print(playerX ? 'X' : 'O');
        tft.println(" Wins!");
        delay(3000);
        tft.fillScreen(BLACK);
        drawGrid();
        resetBoard();
      } else if (isBoardFull()) {
        tft.fillScreen(BLACK);
        tft.setTextColor(WHITE);
        tft.setTextSize(3);
        tft.setCursor(60, 100);
        tft.print("Draw Game!");
        delay(3000);
        tft.fillScreen(BLACK);
        drawGrid();
        resetBoard();
      } else {
        playerX = !playerX; // Switch player
      }
    }

    delay(300);
  }
}

// Draws the 3x3 Tic Tac Toe grid lines
void drawGrid() {
  tft.drawLine(80, 0, 80, 240, WHITE);    // Vertical line 1
  tft.drawLine(160, 0, 160, 240, WHITE);  // Vertical line 2
  tft.drawLine(0, 80, 240, 80, WHITE);    // Horizontal line 1
  tft.drawLine(0, 160, 240, 160, WHITE);  // Horizontal line 2
}

// Draws 'X' or 'O' in the specified cell
void drawSymbol(int row, int col, char symbol) {
  int x = col * 80 + 30;  // X position of symbol
  int y = row * 80 + 30;  // Y position of symbol
  tft.setTextColor(symbol == 'X' ? RED : BLUE); // Color for X or O
  tft.setTextSize(4);  // Text size for symbol
  tft.setCursor(x, y);  // Set cursor to position
  tft.print(symbol);   // Draw symbol
}

// Clears the board and sets all cells to empty
void resetBoard() {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      board[r][c] = ' ';
  playerX = true;  // Start with player X
}

// Checks if there's a winning combination on the board
bool checkWinner() {
  for (int i = 0; i < 3; i++) {
    // Check rows
    if (board[i][0] != ' ' &&
        board[i][0] == board[i][1] &&
        board[i][1] == board[i][2]) return true;

     // Check columns
    if (board[0][i] != ' ' &&
        board[0][i] == board[1][i] &&
        board[1][i] == board[2][i]) return true;
  }
   // Check diagonals
  if (board[0][0] != ' ' &&
      board[0][0] == board[1][1] &&
      board[1][1] == board[2][2]) return true;

  if (board[0][2] != ' ' &&
      board[0][2] == board[1][1] &&
      board[1][1] == board[2][0]) return true;

  return false;  // No winner
}
// Checks if all cells are filled
bool isBoardFull() {
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      if (board[r][c] == ' ')
        return false; // At least one cell is empty
  return true; // All cells are filled
}
