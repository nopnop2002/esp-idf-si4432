# MQTT Example   
This is si4432 and MQTT gateway application.   
```
            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
            |Publisher |--(MQTT)-->|  Broker  |--(MQTT)-->|  ESP32   |--(SPI)--->|  si4432  |==(Radio)==>
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+

            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
==(Radio)==>|  si4432  |--(SPI)--->|  ESP32   |--(MQTT)-->|  Broker  |--(MQTT)-->|Subscriber|
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+
```



# Configuration
![Image](https://github.com/user-attachments/assets/71f0c65d-28b7-4604-a076-4e6017cb3630)
![Image](https://github.com/user-attachments/assets/f2081ef6-8eb7-4e82-b275-e08a8ad45623)

## WiFi Setting
Set the information of your access point.   
![Image](https://github.com/user-attachments/assets/44cab988-c869-431b-93bc-7ab4de2550c2)

## Radioi Setting
Set the wireless communication direction.   
![Image](https://github.com/user-attachments/assets/748d0902-1447-488d-9969-7278f463fdaa)

### MQTT to Radio   
Subscribe with MQTT and send to Radio.   
You can use mosquitto_pub as Publisher.   
```mosquitto_pub -h broker.emqx.io -p 1883 -t "/topic/radio/test" -m "test"```

```
            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
            |Publisher |--(MQTT)-->|  Broker  |--(MQTT)-->|  ESP32   |--(SPI)--->|  si4432  |==(Radio)==>
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+
```


Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC1101_receive   


### Radio to MQTT   
Receive from Radio and publish as MQTT.   
You can use mosquitto_sub as Subscriber.   
```mosquitto_sub -d -h broker.emqx.io  -t "/topic/radio/test"```

```
            +----------+           +----------+           +----------+           +----------+
            |          |           |          |           |          |           |          |
==(Radio)==>|  si4432  |--(SPI)--->|  ESP32   |--(MQTT)-->|  Broker  |--(MQTT)-->|Subscriber|
            |          |           |          |           |          |           |          |
            +----------+           +----------+           +----------+           +----------+
```


Communicate with Arduino Environment.   
Run this sketch.   
ArduinoCode\CC1101_transmitte   

## Broker Setting
Set the information of your MQTT broker.   
![Image](https://github.com/user-attachments/assets/f8daf122-21dc-455e-a69f-cfe5026c7ffa)

### Select Transport   
This project supports TCP,SSL/TLS,WebSocket and WebSocket Secure Port.   

- Using TCP Port.   
	TCP Port uses the MQTT protocol.   

- Using SSL/TLS Port.   
	SSL/TLS Port uses the MQTTS protocol instead of the MQTT protocol.   

- Using WebSocket Port.   
	WebSocket Port uses the WS protocol instead of the MQTT protocol.   

- Using WebSocket Secure Port.   
	WebSocket Secure Port uses the WSS protocol instead of the MQTT protocol.   

__Note for using secure port.__   
The default MQTT server is ```broker.emqx.io```.   
If you use a different server, you will need to modify ```getpem.sh``` to run.   
```
chmod 777 getpem.sh
./getpem.sh
```

WebSocket/WebSocket Secure Port may differ depending on the broker used.   
If you use a different MQTT server than the default, you will need to change the port number from the default.   

### Specifying an MQTT Broker   
You can specify your MQTT broker in one of the following ways:   
- IP address   
 ```192.168.10.20```   
- mDNS host name   
 ```mqtt-broker.local```   
- Fully Qualified Domain Name   
 ```broker.emqx.io```

You can use this as broker.   
https://github.com/nopnop2002/esp-idf-mqtt-broker

### Select MQTT Protocol   
This project supports MQTT Protocol V3.1.1/V5.   
![Image](https://github.com/user-attachments/assets/4a4f63ca-4925-43ff-81c2-8a148a5a6d8f)

### Enable Secure Option   
Specifies the username and password if the server requires a password when connecting.   
[Here's](https://www.digitalocean.com/community/tutorials/how-to-install-and-secure-the-mosquitto-mqtt-messaging-broker-on-debian-10) how to install and secure the Mosquitto MQTT messaging broker on Debian 10.   
![Image](https://github.com/user-attachments/assets/509eddd5-246b-4cab-8644-f45f85d129e1)
