# HTTP Example   
This is si4432 and HTTP gateway application.   
```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            |HTTP Client|--(HTTP)-->|   ESP32   |--(SPI)--->|  si4432   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+

            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  si4432   |--(SPI)--->|   ESP32   |--(HTTP)-->|HTTP Server|
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```



# Configuration
![Image](https://github.com/user-attachments/assets/f29db282-384b-4749-98c4-aac681c60dcb)
![Image](https://github.com/user-attachments/assets/53d9088b-1aaa-4d37-ac51-baa933724a13)

## WiFi Setting
Set the information of your access point.   
![Image](https://github.com/user-attachments/assets/5c0f3850-8699-414f-a130-df088db4e87c)

## Radioi Setting
Set the wireless communication direction.   

### HTTP to Radio
Receive from HTTP and send to Radio.   
ESP32 acts as HTTP Server.   
You can use curl as HTTP Client.   
```sh ./http-client.sh```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
            |HTTP Client|--(HTTP)-->|   ESP32   |--(SPI)--->|  si4432   |==(Radio)==>
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![Image](https://github.com/user-attachments/assets/be869b5e-4a8e-4310-bc93-ae7c24188351)

Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-si4432/tree/main/ArduinoCode/si4432_rx).   

### Radio to HTTP
Receive from Radio and send to HTTP.   
ESP32 acts as HTTP Client.   
You can use nc(netcat) as HTTP Server.   
```sh ./http-server.sh```

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  si4432   |--(SPI)--->|   ESP32   |--(HTTP)-->|HTTP Server|
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

![Image](https://github.com/user-attachments/assets/80e4e4e5-5451-460f-823e-b0139f6d5bd0)


Communicate with Arduino Environment.   
I tested it with [this](https://github.com/nopnop2002/esp-idf-si4432/tree/main/ArduinoCode/si4432_tx).   

### Specifying an HTTP Server   
You can specify your HTTP Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```http-server.local```   
- Fully Qualified Domain Name   
 ```http-server.public.io```


