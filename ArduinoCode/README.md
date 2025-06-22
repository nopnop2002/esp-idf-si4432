# Example of Arduino environment   
I use [this](https://github.com/nopnop2002/Arduino-si4432).   

# Wirering

|SI4432||ATmega328|ATmega2560|ESP8266|
|:-:|:-:|:-:|:-:|:-:|
|CSN|--|D10(*2)|D10(*2)|D15|
|MISO|--|D12|D12|D12|
|MOSI|--|D11(*2)|D11(*2)|D13|
|SCK|--|D13(*2)|D13(*2)|D14|
|SDN|--|D7(*2)|D7(*2)|D4|
|IRQ|--|D2|D2|D5|
|VCC|--|3.3V(*1)|3.3V|3.3V|
|GND|--|GND|GND|GND|

(*1)   
UNO's 3.3V output can only supply 50mA.   
In addition, the output current capacity of UNO-compatible devices is smaller than that of official products.   
__So this module may not work normally when supplied from the on-board 3v3.__   

(*2)    
SI4432 is not 5V tolerant.   
You need level shift from 5V to 3.3V.   
I used [this](https://www.ti.com/lit/ds/symlink/txs0108e.pdf?ts=1647593549503) for a level shift.   

# Configuration

- Radio frequency   
	Specifies the frequency to be used.
	```
	#define FREQUENCY 433
	```

- Radio channel   
	Specifies the channel to be used.
	```
	#define CHANNEL 0
	```

# Application   
- si4432_tx   
	Send only.   
- si4432_rx   
	Receive only.   
- si4432_primary   
	Wait for a response after sending.   
- si4432_secondary   
	Send a response after receiving.   
