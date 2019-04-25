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
const uint8_t S  = A2; //S is digital pot select-ADC2
int adc_1p = A0;
int adc_1n = A1;
int val_adc1p;
int val_adc1n;
int val_adc1_diff;
uint8_t myPins[] = {Q0, Q1, Q2, Q3, Q4, Q5, RA, RB, RC, RD, RE, S};
bool DEBUG = false;
char buf[20];

// EXPERIMENTAL VARIABLES FOR MAHSA
const uint8_t AVG = 10;
const uint8_t STEP_SIZE = 5;
const uint8_t S_INITIAL = 128;
const uint8_t B_INITIAL = 255;
// END EXPERIMENTAL VARIABLES

void setup() {
  Serial.begin(115200);
  pinSetup();
  // initialize SPI:
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE3));

  potWrite(4, 1, 1, 0, S_INITIAL);
  potWrite(4, 1, 1, 1, B_INITIAL);
  delay(10);

}

void loop() {
  receive_comm();
}

void receive_comm() {
  if (Serial.available() > 0) {
    int inChar = Serial.read();
    if      (inChar == 'S') {
      sweep_S();
      flush_serial();
    }
    else if (inChar == 'B') {
      sweep_B();
      flush_serial();
    }
  }
}

void sweep_S() {
//  Serial.println("--- STARTING S SWEEP ---");
  // ZERO THE GATE
  potWrite(4, 1, 1, 1, B_INITIAL);
  // SWEEP VOLTAGE DIVIDER
  delay(50);
  for (int i = 0; i <= 255 ; i += STEP_SIZE) {
    potWrite(4, 1, 1, 0, i);
    delay(50);
    // AVERAGED READINGS
    val_adc1p = avg_read(adc_1p);
    val_adc1n = avg_read(adc_1n);
    val_adc1_diff = val_adc1p - val_adc1n;
    // PAYLOAD
    sprintf(buf, "S%dP%dN%dD%d", i, val_adc1p, val_adc1n, val_adc1_diff);
    Serial.println(buf);
    delay(50);
  }
}

void sweep_B() {
//  Serial.println("--- STARTING B SWEEP ---");
  // EVEN DRAIN VOLTAGES
  potWrite(4, 1, 1, 0, S_INITIAL);
  // SWEET GATE
  for (int i = 0; i <= 255 ; i += STEP_SIZE) {
    potWrite(4, 1, 1, 1, i);
    delay(50);
    // AVERAGED READINGS
    val_adc1p = avg_read(adc_1p);
    val_adc1n = avg_read(adc_1n);
    val_adc1_diff = val_adc1p - val_adc1n;
    // PAYLOAD
    sprintf(buf, "S%dP%dN%dD%d", i, val_adc1p, val_adc1n, val_adc1_diff);
    Serial.println(buf);
    delay(50);
  }
}

void flush_serial() {
  while (Serial.available() > 0) int c = Serial.read();
}

int avg_read(int pin) {
  int sum = 0;
  for (int i = 0; i < AVG; i++) {
    sum += analogRead(pin);
    delay(1);
  }
  return (sum / AVG);
}

void pinSetup() {
  for (int i = 0; i < sizeof(myPins) ; i++) {
    pinMode(myPins[i], OUTPUT);
  }
  pinMode(S, OUTPUT);
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
     s     | Cell Select     | {S,B} -> {0,1}
     res   | 8bit Resistance | {0, ..., 255}
  */
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
  // Row Logic
  digitalWrite(myPins[5 + row], HIGH);
  // S/B Logic
  digitalWrite(S, s);
  // By this point, the CS pin should be selected
  delay(10);
  // Send address and value
  SPI.transfer(res);
  delay(10);
  pinLow();
}
