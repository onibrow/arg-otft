#include <SPI.h>

// Pin Declarations
const uint8_t Q0 = 10;
const uint8_t Q1 = 9;
const uint8_t Q2 = 8;
const uint8_t Q3 = 7;
const uint8_t Q4 = 6;
const uint8_t Q5 = 5;
const uint8_t RA = 1;
const uint8_t RB = 0;
const uint8_t RC = 4;
const uint8_t RD = 3;
const uint8_t RE = 2;
const uint8_t S  = A2;
uint8_t myPins[] = {Q0, Q1, Q2, Q3, Q4, Q5, RA, RB, RC, RD, RE, S};
bool DEBUG = false;

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
    Serial.println("Starting Potentiometer Test...\n");
  }
  pinSetup();
  // initialize SPI:
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE3));
  delay(10);
}

void loop() {

  // Q4 Row 2
  // B's
  potWrite(4, 1, 3, 0, 120);

  // S's
  potWrite(4, 1, 1, 1, 32);
  potWrite(4, 1, 2, 1, 64);
  potWrite(4, 1, 3, 1, 96);
  potWrite(4, 1, 4, 1, 128);
  potWrite(4, 1, 5, 1, 160);

  // Q4 Row 1
  // B's
  potWrite(4, 0, 1, 0, 128);
  potWrite(4, 0, 2, 0, 128);
  potWrite(4, 0, 3, 0, 128);
  potWrite(4, 0, 4, 0, 128);
  potWrite(4, 0, 5, 0, 128);
  // S's
  potWrite(4, 0, 1, 1, 160);
  potWrite(4, 0, 2, 1, 128);
  potWrite(4, 0, 3, 1, 96);
  potWrite(4, 0, 4, 1, 64);
  potWrite(4, 0, 5, 1, 32);

}

void pinSetup() {
  for (int i = 0; i < sizeof(myPins); i++) {
    pinMode(myPins[i], OUTPUT);
  }
  pinLow();
}

void pinLow() {
  for (int i = 0; i < sizeof(myPins); i++) {
    digitalWrite(myPins[i], LOW);
  }
}

void selPin(uint8_t x, uint8_t y, uint8_t z) {
  digitalWrite(x, HIGH);
  digitalWrite(y, HIGH);
  digitalWrite(z, HIGH);
}

void potWrite(uint8_t quad, uint8_t col, uint8_t row, uint8_t s, uint8_t res) {
  /*
     Selects the cell specified by the function

     Input | Name            | Values
     ----- | --------------- | ------
     quad  | Quadrant number | {1,2,3,4}
     col   | Column number   | {1, 2} -> {0,1}
     row   | Row number      | {A,B,C,D,E} -> {1,2,3,4,5}
     s     | Cell Select     | {B,S} -> {0,1}
     res   | 8bit Resistance | {0, ..., 255}
  */
  if (DEBUG) {
    Serial.print("\nWriting ");
    Serial.print(res);
    Serial.print(" to Quadrant ");
    Serial.print(quad);
    Serial.print(", Column ");
    Serial.print(col);
    Serial.print(", Row ");
    Serial.print(row);
    Serial.print(", Cell ");
    Serial.println(s);
  }
  pinLow();

  // Quadrant and Column Logic
  switch (quad) {
    case 1:
      if (col) selPin(Q0, Q1, Q2);
      else     selPin(Q0, Q1, Q3);
      break;
    case 2:
      if (col) selPin(Q0, Q2, Q3);
      else     selPin(Q1, Q2, Q3);
      break;
    case 3:
      if (col) selPin(Q4, Q1, Q2);
      else     selPin(Q4, Q1, Q3);
      break;
    case 4:
      if (col) selPin(Q4, Q2, Q3);
      else     selPin(Q5, Q2, Q3);
      break;
    default:
      break;
  }

  // S/B Logic
  if (s == 1) {
    digitalWrite(S, HIGH);
  }

  // Row Logic
  digitalWrite(myPins[5 + row], HIGH);

  // By this point, the CS pin should be selected
  delay(100);
  // Send address and value
  SPI.transfer(res);
  delay(100);
  pinLow();
}
