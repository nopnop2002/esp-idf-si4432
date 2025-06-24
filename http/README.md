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
![config-wifi](https://github.com/nopnop2002/esp-idf-cc1101/assets/6020549/5f59f5a9-ca9e-488a-90bb-4929065e43d0)

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

![config-radio-1](https://github.com/nopnop2002/esp-idf-cc1101/assets/6020549/26f4bf6f-a0e5-4038-a617-b4fb83badc96)

Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC1101_receive   


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

![config-radio-2](https://github.com/nopnop2002/esp-idf-cc1101/assets/6020549/85cfe7eb-e148-49b6-a9d6-524c321c13d5)


Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC1101_transmitte   


### Specifying an HTTP Server   
You can specify your HTTP Server in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```http-server.local```   
- Fully Qualified Domain Name   
 ```http-server.public.io```


