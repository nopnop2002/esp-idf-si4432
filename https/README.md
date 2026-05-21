# HTTPS Example   
This is si4432 and HTTPS gateway application.   
Receive from si4432 and send to HTTPS Server.   
ESP32 acts as HTTPS Client.   

```
            +-----------+           +-----------+            +------------+
            |           |           |           |            |            |
==(Radio)==>|  si4432   |--(SPI)--->|   ESP32   |--(HTTPS)-->|HTTPS Server|
            |           |           |           |            |            |
            +-----------+           +-----------+            +------------+
```

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-si4432
cd esp-idf-si4432/https/
chmod 777 mkkey.sh
./mkkey.sh
idf.py menuconfig
idf.py flash
```

mkkey.sh creates a server certificate file.   
The server certificate file is associated with the HTTPS server's IP address.   
mkkey.sh automatically retrieves the IP address of the server on which the script is executed and treats that address as an HTTPS server.   
To manually configure the HTTPS server's IP address, modify the script as follows:   
```
IP="192.168.0.123"
openssl req -x509 -new -nodes -key server.key -subj "/CN=${IP}" -days 10000 -out server.crt
```


# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/7d7bee47-e595-463e-a378-11ccd344fe6b" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/62eb268f-d9d4-45bd-bdbd-a02d03d45887" />

## WiFi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/3a2e89ef-f571-4b72-8763-ab97140afd67" />

## Server Setting
Set the information of your HTTPS server.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/b5b9ab50-31e6-48c7-b722-08bfb3fd9c3b" />

# Start the HTTPS server
```
python3 https-server.py
```
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/eedccac9-17e6-4e47-976a-728e6ad9beb4" />

