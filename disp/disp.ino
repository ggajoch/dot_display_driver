#include "SPI.h"

#include "font-5x7.h"

byte latchPin = 8;
byte enablePin = 13;
// SPI MOSI + SCK for data shift

constexpr uint8_t COLUMNS = 28;
constexpr uint8_t ROWS = 7;

uint8_t columns_to_address[COLUMNS] = {
  0b01100,
  0b01110,
  0b01011,
  0b01010,
  0b01001,
  0b01101,
  0b01111,
  0b10100,
  0b10011,
  0b10110,
  0b10010,
  0b10001,
  0b10101,
  0b10111,
  0b11111,
  0b11101,
  0b11110,
  0b11100,
  0b11011,
  0b11010,
  0b11001,
  0b00111,
  0b00101,
  0b00110,
  0b00100,
  0b00011,
  0b00010,
  0b00001,
};

uint8_t rows_set_to_address[ROWS] = {
  0b01111,
  0b01110,
  0b01101,
  0b01100,
  0b01011,
  0b01010,
  0b01001
};

uint8_t rows_clear_to_address[ROWS] = {
  0b10111,
  0b10110,
  0b10101,
  0b10100,
  0b10011,
  0b10010,
  0b10001
};

void setup() {
  Serial.begin(115200);

  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);

  pinMode(latchPin, OUTPUT);

  SPI.begin();

  for(int r = 0; r < ROWS; ++r) {
    for(int c = 0; c < COLUMNS; ++c) {
      clear_pixel(r, c);
    }
  }
}

void send_bytes(uint8_t byteRows, uint8_t byteColumns) {
  byteRows = ~byteRows;
  byteColumns = ~byteColumns;

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE2));
  SPI.transfer(byteColumns);
  SPI.transfer(byteRows);
  SPI.endTransaction();

  delayMicroseconds(10);
  digitalWrite(latchPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(latchPin, LOW);
  delayMicroseconds(10);

  digitalWrite(enablePin, LOW);
  delayMicroseconds(1000);
  digitalWrite(enablePin, HIGH);
}

void clear_pixel(int row, int column) {
  uint8_t byte_row = (0 << 6) + rows_clear_to_address[row];
  uint8_t byte_column = (1 << 6) + columns_to_address[column];

  send_bytes(byte_row, byte_column);
}

void set_pixel(int row, int column) {
  uint8_t byte_row = (1 << 6) + rows_set_to_address[row];
  uint8_t byte_column = (0 << 6) + columns_to_address[column];

  send_bytes(byte_row, byte_column);
}

void write_pixel(int row, int column, bool value) {
  if (row >= ROWS || column >= COLUMNS || column < 0) {
    return;
  }
  if (value) {
    set_pixel(row, column);
  } else {
    clear_pixel(row, column);
  }
}

void print_char(char character, int start_column) {
  for(int r = 0; r < ROWS; ++r) {
      for(int c = 0; c < 5; ++c) {
        int offset = 5*(character - ' ');

        write_pixel(r, start_column + c, Font5x7[offset+c] & (1 << r));
      }
      write_pixel(r, start_column + 5, 0);
    }
}

void loop() {
  char text[] = "HSKRK";

  for(int ch = 0; ch < strlen(text); ++ch) {
    print_char(text[ch], 6*ch);
  }
}