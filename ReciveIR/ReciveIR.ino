/**
 * 超シンプル 赤外線リモコン受信(NECフォーマット)
 */

#define BAUDRATE 9600
#define DATAPIN 2   //       ArduinoUNO:2   M5Stamp:19　RaspberryPi PICO:8

/*
 * 赤外線リモコン受信
 */
#define NECT 562 // クロック基準時間(NECフォーマット)
bool iRrepeatCoomand ;
uint16_t iRAddr ;
uint8_t iRCommand ;

bool readIrData( ) {
  bool retStat = true ;
  uint8_t readData[4] ;
  int setPos = 0 ;
  int bitLen = 0 ;
  int stat = HIGH ;
  uint8_t bitData ;
  unsigned long beginTime ;
  unsigned long passTime ;
  memset(readData,0,sizeof(readData)) ;
  iRrepeatCoomand = false ;

  // ----- データ受信開始待ち -----
  while(digitalRead(DATAPIN) == HIGH) ;

  // ----- FRAMAE受信開始 -----
  beginTime = micros();
  while(digitalRead(DATAPIN) == LOW) ;
  passTime = micros() - beginTime ;
  if (passTime < NECT * 12  || passTime > NECT * 20) {
    return false ;
  }

  // ----- FRAMAE種別 -----
  beginTime = micros();
  while(digitalRead(DATAPIN) == HIGH) ;
  passTime = micros() - beginTime ;
  if (passTime <= NECT * 6) {
    // Repeace FRAME
    iRrepeatCoomand = true ;
    return true ;
  }

  // ----- データ受信 -----
  bitData = 0x01 ;
  beginTime = micros();
  bitLen = 0 ;
  stat = LOW ;
  passTime = 0 ;
  while (bitLen < 32 && passTime < NECT * 6) {
    passTime = micros() - beginTime ;
    if (digitalRead(DATAPIN) !=  stat) {
      if (stat == HIGH) {
        // ----- 1Bit 受信 -----
        if (passTime >= NECT * 2) {
          readData[setPos] |= bitData ;
        }
        if (bitData != 0x80) {
          bitData <<= 1 ;
        } else {
          bitData = 0x01 ;
          setPos ++ ;
        }
        beginTime = micros();
        bitLen ++ ;
        stat = LOW ;
      } else {
        beginTime = micros();
        stat = HIGH ;
      }
    }
  }
  bitData = ~readData[3] ;
  retStat = (bitData == readData[2]) ? true :false ;
  iRAddr = readData[0] << 8 | readData[1] ;
  iRCommand = readData[2] ;

  return retStat ;
}

void setup() {
  Serial.begin(BAUDRATE);
  while(!Serial) ;
  Serial.println("\nIr Recive - Read") ;
  pinMode(DATAPIN, INPUT);
}

void loop() {
  if (readIrData( )) {
    if (iRrepeatCoomand) {
      Serial.print("Repeat ") ;
    }
    Serial.print("Addr:") ;
    Serial.print(iRAddr,HEX) ;
    Serial.print("  Command:") ;
    Serial.println(iRCommand,HEX) ;
  }
  delay(10) ;
}
