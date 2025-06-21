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
  Serial.println((char *)txBuf);
  bool sent = radio.sendPacket(txLen, txBuf);
  if (!sent) {
    Serial.println("sendPacket Fail");
  }
  delay(1000);
}
