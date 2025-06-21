# Ping-Pong Example   
Send data from primary to secondary.   
In the secondary, the characters are converted and sent back.   

- ESP32 is Primary   
```
+-----------+           +-----------+             +-----------+           +-----------+
|           |           |           |             |           |           |           |
|  Primary  |===(SPI)==>|  SI4432   |---(Radio)-->|  SI4432   |===(SPI)==>| Secondary |
|   ESP32   |           |           |             |           |           |           |
|           |           |           |             |           |           |           |
|           |<==(SPI)===|           |<--(Radio)---|           |<==(SPI)===|           |
|           |           |           |             |           |           |           |
+-----------+           +-----------+             +-----------+           +-----------+
```

- ESP32 is Secondary   

```
+-----------+           +-----------+             +-----------+           +-----------+
|           |           |           |             |           |           |           |
|  Primary  |===(SPI)==>|  SI4432   |---(Radio)-->|  SI4432   |===(SPI)==>| Secondary |
|           |           |           |             |           |           |   ESP32   |
|           |           |           |             |           |           |           |
|           |<==(SPI)===|           |<--(Radio)---|           |<==(SPI)===|           |
|           |           |           |             |           |           |           |
+-----------+           +-----------+             +-----------+           +-----------+
```

# Configuration   
![Image](https://github.com/user-attachments/assets/757cc970-e36d-40bb-9b48-e3d46b6a6474)
![Image](https://github.com/user-attachments/assets/f57288c9-6db4-47a8-b289-3af57583ff27)

# Communication with the Arduino environment   
- ESP32 is the primary   
I tested it with [this](https://github.com/nopnop2002/esp-idf-si4432/tree/main/ArduinoCode/si4432_secondary).   

- ESP32 is the secondary   
I tested it with [this](https://github.com/nopnop2002/esp-idf-si4432/tree/main/ArduinoCode/si4432_primary).   


