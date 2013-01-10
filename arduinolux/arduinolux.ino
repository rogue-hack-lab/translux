int clockPin = 12;
int txPin = 11;
int rowEnablePin = 13;

int clockDelay = 100;
int holdCycles = 100;
int rowHoldDelay = 500;
int numColumns = 80;
int columnValues[80];
int rowValue = 1;

void setup() {
  pinMode(clockPin, OUTPUT);
  pinMode(txPin, OUTPUT);
  pinMode(rowEnablePin, OUTPUT);
  
  for (int i=0; i<numColumns; i++) { 
    columnValues[i]=(i%2 ? HIGH : LOW); 
  }
}


void writeBit(int bitValue) {
  digitalWrite(clockPin, LOW);
  delay(2);
  digitalWrite(txPin, bitValue);
  delay(2);
  digitalWrite(clockPin, HIGH);
  delay(2);
  digitalWrite(clockPin, LOW);
}

void enableColumn() {
  digitalWrite(rowEnablePin, HIGH);
}

void disableColumn() {
  digitalWrite(rowEnablePin, LOW);
}

int rowSelect = 0;

void loop() {
  disableColumn();
  delay(2);
  
  // 83 bit frame, 3 trailing bits are the row index of 1 - 7
  for (int columnIndex=0; columnIndex<numColumns; columnIndex++) {
    writeBit(columnValues[columnIndex]);
  }
  rowValue++;
  if (rowValue > 7) { rowValue = 1; }
  
  writeBit(rowValue & 0x1);
  writeBit((rowValue & 0x2)>>1);
  writeBit((rowValue & 0x4)>>2);

  enableColumn();
  
  delay(rowHoldDelay);
}

/*  
void loop() {
  // Ok, try to send bytes 1 - 255 as a bit pattern on txPin
  // for some number of cycles, then start over
  for (int byteToSend=1; byteToSend<256; byteToSend++) {
    //digitalWrite(clockPin, LOW)
    for (int bitToSend=0; bitToSend < 8; bitToSend++) {
        //digitalWrite(clockPin, LOW);
        digitalWrite(txPin, (byteToSend & (1<<bitToSend) ? HIGH : LOW));
        delay(2); // just to make sure clock stays high for long enough
        digitalWrite(clockPin, HIGH);
        delay(2);
        digitalWrite(clockPin, LOW);
    }
    delay(clockDelay);
  }
}
*/

