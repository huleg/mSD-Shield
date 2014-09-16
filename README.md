# mSD-Shield
The mSD-Shield is a plug-on module with microSD socket, RTC (real time clock) with battery backup, prototyping area and optional 2.8" TFT-Display with Touch-Panel for Arduino.

Shop: [mSD-Shield v2](http://www.watterott.com/en/mSD-Shield-v2-Data-Logging-Shield)

![mSD-Shield](https://raw.github.com/watterott/mSD-Shield/master/img/msd-shield.jpg)


## Features
* microSD socket (SPI)
* DS1307 RTC (I2C Real-Time-Clock) with backup battery
* Optional MI0283QT-Adapter (2.8" 320x240 Display with Touch-Panel)
* Prototyping Area


## Hardware
* ![mSD-Shield hardware dia](https://raw.github.com/watterott/mSD-Shield/master/img/hw_dia.png)
* [Schematics + Layout](https://github.com/watterott/mSD-Shield/tree/master/pcb)
* **mSD-Shield v2.x**: For Arduino Uno, Leonardo and Mega with R3-Layout (SDA+SCL next to AREF)
* **mSD-Shield v1.x**: For Arduino Uno. On Mega/Leonardo boards only Software-SPI and no RTC without hardware modifications.
* **Mega mSD-Shield v1.x**: For Arduino Uno and Mega. Uno/Mega adjustment via jumper.


## Software
* [Arduino Library with Examples](https://github.com/watterott/Arduino-Libs)
* [Old Arduino Library with Examples](https://github.com/watterott/mSD-Shield/tree/5054db114faef1bcfd9c1d165ed713a681a0edea/src)
