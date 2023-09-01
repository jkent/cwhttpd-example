This is an example of how to use [cwhttpd](https://github.com/jkent/cwhttpd) with the Espressif FreeRTOS SDK [ESP-IDF](https://github.com/espressif/esp-idf).

## Requirements

| Supported Targets | ESP32 | ESP32-C2 | ESP32-S2 | ESP32-S3 |
|--------|--------|--------|--------|--------|
| ESP-IDF 4.x | ? | ? | ? | ? |
| ESP-IDF 5.0.x | WIP | ? | ? | ? | 
| ESP-IDF 5.1.x | WIP | ? | ? | ? | 

## How to build
Start up your ESP-IDF environment, i.e. in Linux:
```sh
IDF_PATH=~/esp/esp-idf-v5.1.1
source ${IDF_PATH}/export.sh
```

Configure the example and components.  See sections 
 - `Component config -> Clockwise HTTPd` and 
 - `Component config -> FrogFS`
```sh
idf.py menuconfig
```

