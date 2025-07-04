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
  radio.startListening();
}

void loop() {
  byte rxBuf[64] = {0};
  byte rxLen = 0;

  bool recv = radio.isPacketReceived();
  if (recv) {
    radio.getPacketReceived(&rxLen, rxBuf);
    Serial.print("rxLen:");
    Serial.println(rxLen, DEC);

    for (int i = 0; i < rxLen; ++i) {
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    for (int i = 0; i < rxLen; ++i) {
      char c = rxBuf[i];
      if (c > 0x20 && c < 0xFF) {
        Serial.print(c);
      } else {
        Serial.print(" ");
      }
    }
    Serial.println();

    radio.startListening(); // restart the listening.
  }

}
