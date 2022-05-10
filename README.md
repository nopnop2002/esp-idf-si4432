# esp-idf-si4432
SI4432 ISM Transceiver driver for esp-idf.

![SI4432-433MHz-2 54mm](https://user-images.githubusercontent.com/6020549/163330289-770357cd-5bb4-4030-9347-edd0da6f21d3.JPG)

I refered [this](https://github.com/ADiea/si4432).


# Software requirements
esp-idf v4.4 or later.   
This is because this version supports ESP32-C3.   

# Installation

```Shell
git clone https://github.com/nopnop2002/esp-idf-si4432
cd esp-idf-si4432
idf.py set-target {esp32/esp32s2/esp32s3/esp32c3}
idf.py menuconfig
idf.py flash
```

__Note for ESP32C3__   
For some reason, there are development boards that cannot use GPIO06, GPIO08, GPIO09, GPIO19 for SPI clock pins.   
According to the ESP32C3 specifications, these pins can also be used as SPI clocks.   
I used a raw ESP-C3-13 to verify that these pins could be used as SPI clocks.   


# Configuration for Transceiver   

![config-si4432-1](https://user-images.githubusercontent.com/6020549/163330599-b5df4472-3e60-43c8-8507-5ba3fa829dd0.jpg)
![config-si4432-2](https://user-images.githubusercontent.com/6020549/167548307-9a24477b-b598-4e05-affc-e384bc8fda3c.jpg)

# SPI BUS selection   
![config-si4432-3](https://user-images.githubusercontent.com/6020549/167548340-f363c727-fef5-4dfe-954f-fc2ceb99627c.jpg)

The ESP32 series has three SPI BUSs.   
SPI1_HOST is used for communication with Flash memory.   
You can use SPI2_HOST and SPI3_HOST freely.   
When you use SDSPI(SD Card via SPI), SDSPI uses SPI2_HOST BUS.   
When using this module at the same time as SDSPI or other SPI device using SPI2_HOST, it needs to be changed to SPI3_HOST.   
When you don't use SDSPI, both SPI2_HOST and SPI3_HOST will work.   
It was called HSPI_HOST / VSPI_HOST, but it is not defined in SoCs other than ESP32.   

# Configuration for Application   

![config-top](https://user-images.githubusercontent.com/6020549/163330706-44e26f06-c4a7-4cd0-aa5d-1c59db3d176d.jpg)
![config-app](https://user-images.githubusercontent.com/6020549/163330711-2db45e5d-34ae-40f5-bab7-2fef1e7af122.jpg)

- Enable Ping-Pong   
Send data from TX to RX.   
In the RX, the characters are converted and sent back to TX.   
TX waits for a response from RX.   
![Ping-Pong](https://user-images.githubusercontent.com/6020549/163336025-25b18572-5dde-49e0-ada3-302f3a8c5eb8.jpg)


# Wirering

|SI4432||ESP32|ESP32-S2/S3|ESP32-C3|
|:-:|:-:|:-:|:-:|:-:|
|MISO|--|GPIO19|GPIO37|GPIO18|
|SCK|--|GPIO18|GPIO36|GPIO10|
|MOSI|--|GPIO23|GPIO35|GPIO19|
|SEL|--|GPIO5|GPIO34|GPIO7|
|SDN|--|GPIO15|GPIO40|GPIO8|
|IRQ|--|GPIO16|GPIO41|GPIO9|
|GND|--|GND|GND|GND|
|VCC|--|3.3V|3.3V|3.3V|

__You can change it to any pin using menuconfig.__   

# Screen Shot

![ScrrenShot-1](https://user-images.githubusercontent.com/6020549/167547945-56c0038f-38ee-4801-baef-0830fc4382d4.jpg)
![ScrrenShot-2](https://user-images.githubusercontent.com/6020549/167547876-65e1c3c0-a352-4949-b2ed-96819a0e8dfc.jpg)



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

