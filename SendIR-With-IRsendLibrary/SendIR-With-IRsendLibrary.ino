/**
 * SEND IR COMAND With M5Atom
 * for DISO 100YEN LED COLOR LAMP
 * with IRsend Libtrary
 */

#include <IRsend.h>
#define BUTTONPIN 39 // M5Atom BUTTON

const uint16_t kIrLed = 12 ; // 12:M5Atom IR LED GPIO pin
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

uint32_t dataPowerON  = 0x807F12ED ;
uint32_t dataPowerOFF = 0x807F1AE5  ;

bool switchStatus = false ; 

// swap the order of bits
uint32_t makeRowData(uint32_t cmd) {
  uint32_t cmdR = 0 ;
  uint8_t *dataP = (uint8_t *)&cmd ;
  uint8_t *dataR = (uint8_t *)&cmdR ;

  for (int i=0;i<4;i++) {
    uint8_t bit1 = 0x01 ;
    uint8_t bit2 = 0x80 ;
    for (int j=0;j<8;j++) {
      if ((dataP[i] & bit1) != 0) {
        dataR[i] |= bit2 ;
      }
      bit1 <<= 1 ;
      bit2 >>= 1 ;
    }
  }
  return cmdR ;
}

void setup() {
  irsend.begin();
  pinMode(BUTTONPIN,INPUT) ;
}

void loop() {
  if (digitalRead(BUTTONPIN) == LOW) {
    if (switchStatus) {
      switchStatus = false ;
      uint32_t cmd = makeRowData(dataPowerOFF);
      irsend.sendNEC(cmd ,32);
    } else {
      switchStatus = true ;
      uint32_t cmd = makeRowData(dataPowerON);
      irsend.sendNEC(cmd ,32);
    }
  }
  delay(300) ;
}
