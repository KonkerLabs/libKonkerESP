# KonkerESP

## Usage

Include _konker.h_ to work with this library.

### Initializing

Declare _KonkerDevice_ object:

    KonkerDevice device;

Initialize konker lib at `setup()`, there are several possible configurations to be changed:
- `device.addWifi()`
- `device.setServer()`
- `device.setPlatformCredentials()`
- `device.setDefaultConnectionType()`

Inialize WiFi connection and connection to platform:

- `device.init()`

__Example__:

    void setup(){
        Serial.println("Setup");

        device.addWifi("ssid", "password");
        device.setDefaultConnectionType(ConnectionType::MQTT);
        device.setServer("prod.konkerlabs.net", 1883);

        device.init();

        Serial.println("Setup finished");
    }

After the setup in the main `loop()` put:

    device.loop();

__Example__:

    void loop(){
        device.loop();
    }

### Sending data

Data collected is stored in a buffer, from where it is sent to the platform.

__Example__:

    // message is a json formatted string
    device.storeData("pub_channel", message);

    device.sendData();

### Device credentials

TODO

### Firmware updates

Updates checks are done automatically inside the `KonkerDevice::loop()` function. It is not checked every loop, the interval should be set accordingly. 