# esp-idf-si4432
SI4432 ISM Transceiver driver for esp-idf.   

I refered [this](https://github.com/ADiea/si4432).   

- 2.54mm pitch 433MHz   
	![SI4432-433MHz-2 54mm](https://user-images.githubusercontent.com/6020549/163330289-770357cd-5bb4-4030-9347-edd0da6f21d3.JPG)

- 1.27mm pitch 433MHz   
	![Image](https://github.com/user-attachments/assets/ad63e3e6-c65f-4888-9ede-e26ca1a71905)
	Pinout is [here](http://www.dorji.com/docs/data/DRF4432F20.pdf)   
	I used this pitch conversion PCB.   
	![SI4432-433MHz-1 27mm](https://user-images.githubusercontent.com/6020549/170854841-ab5318ae-7b31-4d11-98d9-719f48a49c36.JPG)

# Software requirements
ESP-IDF V4.4/V5.x.   
ESP-IDF V5.0 is required when using ESP32-C2.   
ESP-IDF V5.1 is required when using ESP32-C6.   

# Installation

```Shell
git clone https://github.com/nopnop2002/esp-idf-si4432
cd esp-idf-si4432/basic
idf.py set-target {esp32/esp32s2/esp32s3/esp32c2/esp32c3/esp32c6}
idf.py menuconfig
idf.py flash
```

__Note for ESP32C3__   
For some reason, there are development boards that cannot use GPIO06, GPIO08, GPIO09, GPIO19 for SPI clock pins.   
According to the ESP32C3 specifications, these pins can also be used as SPI clocks.   
I used a raw ESP-C3-13 to verify that these pins could be used as SPI clocks.   


# Configuration for Transceiver   
![Image](https://github.com/user-attachments/assets/f60383fc-c530-4e38-806b-a69a921044cf)
![Image](https://github.com/user-attachments/assets/d4c85478-7c27-4a46-90b3-58e976c6a6d3)

# SPI BUS selection   
![Image](https://github.com/user-attachments/assets/02d5047c-a397-4e38-8f67-256d2e41e6f0)

The ESP32 series has three SPI BUSs.   
SPI1_HOST is used for communication with Flash memory.   
You can use SPI2_HOST and SPI3_HOST freely.   
When you use SDSPI(SD Card via SPI), SDSPI uses SPI2_HOST BUS.   
When using this module at the same time as SDSPI or other SPI device using SPI2_HOST, it needs to be changed to SPI3_HOST.   
When you don't use SDSPI, both SPI2_HOST and SPI3_HOST will work.   
It was called HSPI_HOST / VSPI_HOST, but it is not defined in SoCs other than ESP32.   

# Frequency and Channel to use   
![Image](https://github.com/user-attachments/assets/733ee4e1-6e0e-4298-8ea8-62bd8dc5b8e1)

The frequency and channel used by the sender and receiver must match.   

# Wirering

|SI4432||ESP32|ESP32-S2/S3|ESP32-C2/C3/C6|
|:-:|:-:|:-:|:-:|:-:|
|MISO|--|GPIO19|GPIO37|GPIO4|
|SCK|--|GPIO18|GPIO36|GPIO3|
|MOSI|--|GPIO23|GPIO35|GPIO2|
|SEL|--|GPIO5|GPIO34|GPIO1|
|SDN|--|GPIO15|GPIO40|GPIO5|
|IRQ|--|GPIO16|GPIO41|GPIO6|
|GND|--|GND|GND|GND|
|VCC|--|3.3V|3.3V|3.3V|

__You can change it to any pin using menuconfig.__   

# Troubleshooting

- In transmit mode.   
	If you get this error, try changing the IRQ GPIO to a another GPIO.   
	```
	W (33939) sendPacket: Timeout in Transit --
	E (4789) TX: sendPacket Fail
	```

- In receive mode.   
	If you can't receive anything, tyr changing the IRQ GPIO to another GPIO.   


# Comparison of nRF905, cc1101 and si4432
||nRF905|cc1101|si4432|
|:-:|:-:|:-:|:-:|
|Manufacturer|Nordic Semiconductor|Texas Instrument|Silicon Labs|
|Frequency|433/868/915MHz|315/433/868/915MHz|315/433/868/915MHz|
|Maximum Payload|32Byte|64Byte|64Byte|
|FiFo for reception|32Byte*1|64Byte*1|64Byte*1|
|FiFo for transmission|32Byte*1|64Byte*1|64Byte*1|
|CRC Length|8/16bits|16bits|8/16bits|
|Acknowledgement Payload|No|No|No|
|Modulation format|GFSK|2-FSK/4-FSK/GFSK/ASK/OOK/MSK|FSK/GFSK/OOK|


- GFSK  : Gaussian shaped Frequency Shift Keying
- FSK   : Frequency Shift Keying
- 2-FSK : Binary Frequency Shift Keying
- 4-FSK : Quaternary Frequency Shift Keying
- ASK   : Amplitude Shift Keying
- OSK   : On-Off Keying
- MSI   : Minimum Shift Keying

