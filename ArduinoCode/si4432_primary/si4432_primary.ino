#include "si4432.h"

#define FREQUENCY 433
#define CHANNEL 0

// for UNO
Si4432 radio(10, 7, 2); // CS, SDN, IRQ

// for MEGA
//Si4432 radio(10, 7, 2); // CS, SDN, IRQ

//for ESP8266
//Si4432 radio(15, 4, 5); // CS, SDN, IRQ

void setup() {
  delay(1000);
  Serial.begin(115200);
  if (radio.init() == false) {
    Serial.println("SI4432 not installed");
    while(1);
  }
  
  radio.setFrequency(FREQUENCY);
  radio.setChannel(CHANNEL);
  radio.setBaudRate(70);
  //radio.readAll();
  //radio.startListening();
}

void loop() {
  byte txBuf[64];
  byte txLen = sprintf((char *)txBuf, "Hello World %lu", millis());
  //Serial.println((char *)txBuf);
  bool sent = radio.sendPacket(txLen, txBuf);
  if (sent) {
    bool waiting = true;
    byte rxLen;
    byte rxBuf[64];
    unsigned long startMillis = millis();
    Serial.println("startListening");
    radio.startListening(); // restart the listening.
    
    while(waiting) {
      bool recv = radio.isPacketReceived();
      if (recv) {
        radio.getPacketReceived(&rxLen, rxBuf);
        unsigned long receiveMillis = millis();
        unsigned long elaspedMills = receiveMillis - startMillis;
        Serial.print(receiveMillis);
        Serial.print(" rxLen:");
        Serial.print(rxLen, DEC);
        Serial.print(" elasped:");
        Serial.print(elaspedMills);
        Serial.println();
        
        for (int i = 0; i < rxLen; ++i) {
          Serial.print(rxBuf[i], HEX);
          Serial.print(" ");
        }
        Serial.println();
    
        for (int i = 0; i < txLen; ++i) {
          char c = txBuf[i];
          if (c > 0x20 && c < 0xFF) {
            Serial.print(c);
          } else {
            Serial.print(" ");
          }
        }
        Serial.print("-->");
        for (int i = 0; i < rxLen; ++i) {
          char c = rxBuf[i];
          if (c > 0x20 && c < 0xFF) {
            Serial.print(c);
          } else {
            Serial.print(" ");
          }
        }
        Serial.println();
        break;
      } // end if

      if ( (millis() - startMillis) > 1000) {
        Serial.println((char *)txBuf);
        Serial.println("No responce within 1000 ms");
        waiting = false;
      }
      
    } // end while

  } else {
    Serial.println("sendPacket Fail");
  }
  delay(1000);
}
