# SSL Example   
This is si4432 and SSL gateway application.   
Receive from si4432 and send to SSL Server.   
ESP32 acts as SSL Client.   

```
            +-----------+           +-----------+           +-----------+
            |           |           |           |           |           |
==(Radio)==>|  si4432   |--(SPI)--->|   ESP32   |--(SSL)--->| SSL Server|
            |           |           |           |           |           |
            +-----------+           +-----------+           +-----------+
```

# Installation
```
git clone https://github.com/nopnop2002/esp-idf-si4432
cd esp-idf-si4432/ssl/
chmod 777 mkkey.sh
./mkkey.sh
idf.py menuconfig
idf.py flash
```

mkkey.sh creates a server certificate file.   
The server certificate file is associated with the SSL server's IP address.   
mkkey.sh automatically retrieves the IP address of the server on which the script is executed and treats that address as an SSL server.   
To manually configure the SSL server's IP address, modify the script as follows:   
```
IP="192.168.0.123"
openssl req -x509 -new -nodes -key server.key -subj "/CN=${IP}" -days 10000 -out server.crt
```


# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/ffa9bde3-8d49-43b1-950a-a0d7f0678d50" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/2f7fc657-6c99-425d-a58b-2fc1b8dfb310" />

## WiFi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/01d5a468-fbf3-4b8f-97a1-83f2a49c004e" />

## Server Setting
Set the information of your SSL server.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/7c8eb7ea-0fcf-4d02-a5fe-73f79d75aa20" />

# Start the SSL server
- C language
	```
	cd clang-tls-communication
	make
	./server
	```
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/84d034f3-d60c-4c92-bceb-a6ff61775ffd" />

- python script
	```
	cd python-tls-communication
	python3 server.py
	```
	<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/c5dfcd87-97ca-4541-8e9c-74916d98f119" />

