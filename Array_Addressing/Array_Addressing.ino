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
char buf[50];

void setup() {

  Serial.begin(115200);
  //Serial.println("Starting Potentiometer Test...\n");
  pinSetup();
  // initialize SPI:
  SPI.begin();
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE3));

  //  for (int i = 0; i < sizeof(myPins) -1 ; i++) {
  //    digitalWrite(myPins[i], HIGH);
  //    delay(10);
  //    digitalWrite(myPins[i], LOW);
  //  }
  delay(100); 
  potWrite(4, 1, 1, 0, 255); //C2 RD (9)
  delay(10);
  potWrite(4, 1, 1, 1, 0); //C2RE (10)
  delay(10);

}

int state = 1;

void loop() {
  //C1 RA_bottomDP0 (1)
  val_adc1p = analogRead(adc_1p);
  val_adc1n = analogRead(adc_1n);
  val_adc1_diff = val_adc1p - val_adc1n;
  //adc is 10-bit max is 1023 (1024 steps)...
  //data to be read:
  //voltage/5V*1024
  sprintf(buf, "ADCp:%d,  ADCn:%d,  ADC_diff:%d ", val_adc1p, val_adc1n, val_adc1_diff);
  Serial.println(buf);
  delay(50);
  /*if (state == 1) {
    //potWrite(4, 1, 1, 1, 150); //C1 RB (2)
    //delay(10);
    //potWrite(4, 1, 1, 0, 128); //C1 RC (3)
    //delay(10);
    //state = 0;
    //potWrite(4, 1, 1, 1, 176); //C1 RB (2)
    //delay(10);
    //potWrite(4, 1, 1, 0, 255); //C1 RC (3)
    //delay(10);
    state = 0;
    }
    //potWrite(4, 0, 4, 0, 128); //C1 RD (4)
    //delay(10);
    //potWrite(4, 0, 5, 0, 255); //C1 RE (5)
    //delay(10);
    //potWrite(4, 1, 1, 0, 16); //C2 RA (6)
    //delay(10);
    //potWrite(4, 1, 2, 0, 32);  //C2 RB (7)
    //delay(10);
    //potWrite(4, 1, 1, 0, 100);  //C2 RB_topDP1 (7)
    //delay(10);
    //potWrite(4, 1, 3, 0, 64);  //C2 RC (8)
    //delay(10);
  */


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
  /*
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
  delay(100);
  // Send address and value
  SPI.transfer(res);
  delay(100);
  pinLow();
}
