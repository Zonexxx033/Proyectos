#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS 42
#define DATA_PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

#define NUM_COLS 7
#define NUM_ROWS 6

int grid[NUM_COLS][NUM_ROWS];
int winningPositions[NUM_COLS][NUM_ROWS];

int currentPlayer = 1;
int dropCount = 0;
int gamewon = 0;

uint32_t colorPlayer1 = strip.Color(0, 0, 255); // Blue
uint32_t colorPlayer2 = strip.Color(255, 0, 0); // Red

// Definición de pines para los botones de cada columna
const int buttonPins[NUM_COLS] = {2, 3, 4, 5, 6, 7, 8};

void setup() {
  Serial.begin(9600);

  // Configuración de botones
  for (int i = 0; i < NUM_COLS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP); // Activa la resistencia interna pull-up
  }

  // Configuración de NeoPixel
  strip.begin();
  strip.show(); // Inicializa todos los píxeles apagados
}


void loop() {
  if (gamewon == 0) {
    int selectedColumn = -1;

    if (Serial.available() > 0) {
      selectedColumn = Serial.parseInt() - 1;
    }

    if (selectedColumn >= 0 && selectedColumn < NUM_COLS) {
      play(selectedColumn);
    }
  }
}

void play(int selectedColumn) {
  int droppedOnRow = drop(selectedColumn, currentPlayer);

  if (droppedOnRow >= 0) {
    showPixels();
    dropCount++;
    gamewon = fourConnected(currentPlayer);

    if (gamewon == 0) {
      currentPlayer = (currentPlayer == 1) ? 2 : 1;

      Serial.print("Player ");
      Serial.print(currentPlayer);
      Serial.print(" selected column ");
      Serial.print(selectedColumn + 1);
      Serial.print(" and dropped on row ");
      Serial.print(droppedOnRow + 1);
      Serial.print(". Total drop count: ");
      Serial.println(dropCount);

      if (dropCount == NUM_COLS * NUM_ROWS) {
        Serial.println("Game ended in a draw.");
        reset();
      }
    } else {
      Serial.print("Player ");
      Serial.print(currentPlayer);
      Serial.println(" wins!");
      showWinningAnimation(currentPlayer);
      delay(2000); // Wait 2 seconds before reset
      reset();
    }
  } else {
    Serial.println("Column is full, retry!");
  }
}

int drop(int column, int player) {
  for (int row = NUM_ROWS - 1; row >= 0; row--) {
    if (grid[column][row] == 0) {
      grid[column][row] = player;
      return row;
    }
  }
  return -1;
}

int fourConnected(int player) {
  // Check horizontal
  for (int j = 0; j < NUM_ROWS; j++) {
    for (int i = 0; i <= NUM_COLS - 4; i++) {
      if (grid[i][j] == player &&
          grid[i + 1][j] == player &&
          grid[i + 2][j] == player &&
          grid[i + 3][j] == player) {
        winningPositions[i][j] = player;
        winningPositions[i + 1][j] = player;
        winningPositions[i + 2][j] = player;
        winningPositions[i + 3][j] = player;
        return 1; // Horizontal win
      }
    }
  }

  // Check vertical
  for (int i = 0; i < NUM_COLS; i++) {
    for (int j = 0; j <= NUM_ROWS - 4; j++) {
      if (grid[i][j] == player &&
          grid[i][j + 1] == player &&
          grid[i][j + 2] == player &&
          grid[i][j + 3] == player) {
        winningPositions[i][j] = player;
        winningPositions[i][j + 1] = player;
        winningPositions[i][j + 2] = player;
        winningPositions[i][j + 3] = player;
        return 1; // Vertical win
      }
    }
  }

  // Check diagonal (descending)
  for (int i = 0; i <= NUM_COLS - 4; i++) {
    for (int j = 0; j <= NUM_ROWS - 4; j++) {
      if (grid[i][j] == player &&
          grid[i + 1][j + 1] == player &&
          grid[i + 2][j + 2] == player &&
          grid[i + 3][j + 3] == player) {
        winningPositions[i][j] = player;
        winningPositions[i + 1][j + 1] = player;
        winningPositions[i + 2][j + 2] = player;
        winningPositions[i + 3][j + 3] = player;
        return 1; // Diagonal win
      }
    }
  }

  // Check diagonal (ascending)
  for (int i = 0; i <= NUM_COLS - 4; i++) {
    for (int j = 3; j < NUM_ROWS; j++) {
      if (grid[i][j] == player &&
          grid[i + 1][j - 1] == player &&
          grid[i + 2][j - 2] == player &&
          grid[i + 3][j - 3] == player) {
        winningPositions[i][j] = player;
        winningPositions[i + 1][j - 1] = player;
        winningPositions[i + 2][j - 2] = player;
        winningPositions[i + 3][j - 3] = player;
        return 1; // Diagonal win
      }
    }
  }

  return 0; // No win
}

void reset() {
  // Lógica para reiniciar el juego Connect Four
  for (int col = 0; col < NUM_COLS; col++) {
    for (int row = 0; row < NUM_ROWS; row++) {
      grid[col][row] = 0;
      winningPositions[col][row] = 0;
    }
  }

  strip.clear();
  strip.show();

  dropCount = 0;
  currentPlayer = 1;
  gamewon = 0;
}

void showPixels() {
  strip.clear();

  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      if (grid[col][row] == 1) {
        strip.setPixelColor(row * NUM_COLS + col, colorPlayer1);
      } else if (grid[col][row] == 2) {
        strip.setPixelColor(row * NUM_COLS + col, colorPlayer2);
      }
    }
  }

  strip.show();
}

void showWinningAnimation(int player) {
  strip.clear();

  uint32_t color = (player == 1) ? colorPlayer1 : colorPlayer2;

  // Flash winning pattern
  for (int i = 0; i < 5; i++) {
    for (int col = 0; col < NUM_COLS; col++) {
      for (int row = 0; row < NUM_ROWS; row++) {
        if (winningPositions[col][row] == player) {
          strip.setPixelColor(row * NUM_COLS + col, color);
        }
      }
    }
    strip.show();
    delay(200);
    strip.clear();
    strip.show();
    delay(200);
  }

  // Blink all pixels in the winning color
  for (int i = 0; i < 5; i++) {
    strip.fill(color, 0, NUM_PIXELS);
    strip.show();
    delay(200);
    strip.clear();
    strip.show();
    delay(200);
  }
}

int checkButtons() {
  // Verifica el estado de los botones y devuelve la columna seleccionada
  for (int col = 0; col < NUM_COLS; col++) {
    if (digitalRead(buttonPins[col]) == LOW) { // Botón activo con LOW (negativo)
      return col;
    }
  }
  return -1; // Ningún botón activo
}
