# IoT_DHT_Sensor
This project shows the use of the Node MCU (ESP 8266) with the DHT sensor

STEPS I TOOK TO GET THE ESP8266 TO START WORKING

1. I noticed that USB to UART BRidge popped up whenever I plugged the NodeMCU
2. I then downloaded and unzipped the driver for it from https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads since it had no driver and it had the yellow warning sign
3. I then updated the driver locally
4. I went back to the PlatformIO / (or Arduino) and I set the COM port to COM7. (might be different on your system)
5. Wipe your tears, Redemption has come!


STEPS TO CONNECT THE PROJECT TO THE THINKSPEAK SERVER
1. Install the Thinkspeak library by Mathworks (which should be available if you clone this repo)
2. Open an account at https://thingspeak.com/
3. Create a new channel - Name it after the project you are working on prefarably
4. Enter the fields - corresponding to the parameters you want to send and track on the server...
5. take note of you channel id & your writeAPIkey
6. The rest is history.

THE SCHEMATICS OF THE PROJECT
![Schematics](https://user-images.githubusercontent.com/68389270/184555289-03e66266-4936-48e9-8f29-5151d27f3ceb.jpg)
