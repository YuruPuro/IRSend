/**
 * SEND IR COMAND With M5Atom
 * for DISO 100YEN LED COLOR LAMP
 */

#define BAUDRATE 9600
#define BUTTONPIN 39 // M5Atom BUTTON
#define DATAPIN 12   // M5Atom (IR PIN)

#define HBYTE(x) ((x >> 8) & 0xFF)
#define LBYTE(x) (x & 0xFF)

// --- DISO COLOR LED LAMP address and command
uint16_t IRAddr = 0x807F ;
uint8_t IRCmdPowerON  = 0x12 ;
uint8_t IRCmdPowerOFF = 0x1A ;
int IRCmdCOLORNum = 9 ;
uint8_t IRCmdCOLOR[] = {0x04,0x05,0x06 ,
                        0x07,0x08,0x09 ,
                        0x0A,0x1B,0x1F ,
                        0x0C,0x0D,0x0E } ;

// Send command in NEC format
void SendIRCommand(uint16_t Addr,uint8_t cmd) {
  int i ,n ,m ;
  uint8_t data[4] = { HBYTE(Addr) , LBYTE(Addr) , cmd , ~cmd } ;

  // 1ms = 38 Loops : T=(562μS)21.356 Loops
  for (i=0;i<341;i++) { // Send 16T(8992μS) - HIGH
      digitalWrite(DATAPIN,HIGH) ;
      delayMicroseconds(13);
      digitalWrite(DATAPIN,LOW) ;
      delayMicroseconds(12);
  }
  for (i=0;i<170;i++) { // Send 8T(4496μS) - LOW
      digitalWrite(DATAPIN,LOW) ;
      delayMicroseconds(13);
      digitalWrite(DATAPIN,LOW) ;
      delayMicroseconds(12);
  }
  // SWND : ADDRESS + DATA + ~DATA
  for (n=0;n<8;n++) {
    uint8_t mask = 0x01 ;
    for (m=0;m<8;m++) {
      int t = ((data[n] & mask) == 0) ? 21 : 64 ;
      for (i=0;i<21;i++) { // Send T - HIGH
        digitalWrite(DATAPIN,HIGH) ;
        delayMicroseconds(13);
        digitalWrite(DATAPIN,LOW) ;
        delayMicroseconds(12);
      }
      for (i=0;i<t;i++) { // Send t - LOW
        digitalWrite(DATAPIN,LOW) ;
        delayMicroseconds(13);
        digitalWrite(DATAPIN,LOW) ;
        delayMicroseconds(12);
      }
      mask <<=1 ;
    }
  }
  // end mark. probably not necessary
  for (i=0;i<21;i++) { // Send T - HIGH
    digitalWrite(DATAPIN,HIGH) ;
    delayMicroseconds(13);
    digitalWrite(DATAPIN,LOW) ;
    delayMicroseconds(12);
  }
}

// --- CONTROAL VALUE ---
int cnt = 0 ;
int color = 0 ;
bool switchStatus = false ; 

// --- INITIAL ---
void setup() {
  Serial.begin(BAUDRATE);
  Serial.println("\nIr Recive - Read") ;
  pinMode(BUTTONPIN,INPUT) ;
  pinMode(DATAPIN, OUTPUT);
  digitalWrite(DATAPIN,LOW) ;
}

// --- LOOP : COLOR CHANGE ---
void loop() {
  if (digitalRead(BUTTONPIN) == LOW) {
    if (switchStatus) {
      switchStatus = false ;
      SendIRCommand(IRAddr,IRCmdPowerOFF) ;
    } else {
      switchStatus = true ;
      SendIRCommand(IRAddr,IRCmdPowerON) ;
    }
  }
  if (switchStatus) {
    cnt = (cnt + 1) % 10 ; 
    if (cnt == 0) {
      SendIRCommand(IRAddr,IRCmdCOLOR[color]) ;
      color = (color + 1) % IRCmdCOLORNum ;
    }
  }
  delay(200) ;
}
