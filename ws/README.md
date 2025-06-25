# WebSocket Example   
This is si4432 and WebSocket gateway application.   
```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            | WS Client |-(Socket)->|   ESP32   |--(SPI)--->|  si4432   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+

            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  si4432   |--(SPI)--->|   ESP32   |-(Socket)->| WS Server |
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```



# Configuration
![Image](https://github.com/user-attachments/assets/00d0280e-f9ca-474b-ba93-4c028e7755a9)
![Image](https://github.com/user-attachments/assets/a829a8e6-c47f-4583-aa8c-232d8b5d572c)

## WiFi Setting
Set the information of your access point.   
![Image](https://github.com/user-attachments/assets/5dfc6cd4-80fb-4ad2-8fe8-4ac5cd31b004)

## Radioi Setting
Set the wireless communication direction.   

### WS to Radio
Receive from WebSocket and send to Radio.   
ESP32 acts as WebSocket Server.   
You can use ws-client.py as WS Client.   
```python3 ws-client.py```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            | WS Client |-(Socket)->|   ESP32   |--(SPI)--->|  si4432   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![Image](https://github.com/user-attachments/assets/d93de4af-5cc9-4b40-acc8-8b62944e190f)

Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-si4432/tree/main/ArduinoCode/si4432_rx).   

### Radio to WS
Receive from Radio and send to WebSocket.   
ESP32 acts as WebSocket Client.   
Use [this](https://components.espressif.com/components/espressif/esp_websocket_client) component.   
You can use ws-server.py as WS Server.   
```python3 ws-server.py```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  si4432   |--(SPI)--->|   ESP32   |-(Socket)->| WS Server |
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![Image](https://github.com/user-attachments/assets/448c68d6-ce99-481b-a5c9-3a222f0d243b)

Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-si4432/tree/main/ArduinoCode/si4432_tx).   

### Specifying an WebSocket Server   
You can specify your WebSocket Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```ws-server.local```   
- Fully Qualified Domain Name   
 ```ws-server.public.io```


