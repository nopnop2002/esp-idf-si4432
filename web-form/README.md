# Web Form Example   
Data entered through the web form is sent to Radio.   
Data received from Radio is displayed in a web form.   
```
            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
            |  WebForm  |-(WebSocket)->|   ESP32   |----(SPI)---->|  si4432   |==(Radio)==>
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+

            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
==(Radio)==>|  si4432   |----(SPI)---->|   ESP32   |-(WebSocket)->|  WebForm  |
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+
```

I used [this](https://github.com/Molorius/esp32-websocket) component.   
This component can communicate directly with the browser.   
It's a great job.   

# Configuration
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/b17ce020-3554-41aa-b6b5-c4b524a089b6" />
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/ba329147-38e4-4307-8b00-85afa17248cb" />

## WiFi Setting
Set the information of your access point.   
<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/9977cbb4-f9df-4dcc-96f9-9a0ffa0027be" />

## Radio Setting
Set the wireless communication direction.   

### Web to Radio
Data entered through the web form is sent to Radio.   
```
            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
            |  WebForm  |-(WebSocket)->|   ESP32   |----(SPI)---->|  si4432   |==(Radio)==>
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+
```

<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/5a7f215f-1bef-4e6b-86aa-920fe91e371f" />


### Radio to Web
Data received from Radio is displayed in a web form.   

```
            +-----------+              +-----------+              +-----------+
            |           |              |           |              |           |
==(Radio)==>|  si4432   |----(SPI)---->|   ESP32   |-(WebSocket)->|  WebForm  |
            |           |              |           |              |           |
            +-----------+              +-----------+              +-----------+
```

<img width="659" height="486" alt="Image" src="https://github.com/user-attachments/assets/812f57df-5687-46e6-bdb5-b7f485816aa8" />

# Launch a web browser
Enter the following in the address bar of your web browser.   
```
http:://{IP of ESP32}/
or
http://esp32-server.local/
```

<img width="1104" height="947" alt="Image" src="https://github.com/user-attachments/assets/55501056-7900-45b7-87f9-4eb00a884a9a" />
<img width="1104" height="947" alt="Image" src="https://github.com/user-attachments/assets/d50dd026-2f2b-4c5b-9784-792406147628" />

### Web to Radio
Enter the data to send in the TextBox and press the Send button.   
<img width="1104" height="947" alt="Image" src="https://github.com/user-attachments/assets/e84a3c4a-d820-456b-bdbe-fb29bd8e28cf" />

### Radio to Web
The received data will be displayed in the TextBox.   
The Change button changes the number of lines displayed.   
The Copy button copies the received data to the clipboard.   
<img width="1104" height="947" alt="Image" src="https://github.com/user-attachments/assets/168db48c-7145-4e15-9f48-81320d72285d" />
<img width="1104" height="947" alt="Image" src="https://github.com/user-attachments/assets/34a73a97-b781-4533-bedc-c61885f8627a" />

# WEB Pages
WEB Pages are stored in the html folder.   
I used [this](https://bulma.io/) open source css.   
You can change root.html as you like.   


