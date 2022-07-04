# KonkerESP

#### https://github.com/KonkerLabs/KonkerESPExamples

Include _konker.h_ or _konkerMQTT.h_  to work with this library

Initialize the konker lib at setup

    konkerConfig(<mqtt server>,<model prefix>,<encryption flag>,<http server>,<http port>);

- `<mqtt server>` : Domain / IP of the Konker platform

> Example: (char*)"data.demo.konkerlabs.net:1883"

- `<model prefix>`: A user defined prefix to especify the type of the device, for example LIGHT, MOTOR, etc.

> Example: (char*)"S0101"

- `<encryption flag>`: Boolean flag.  If set true, the WiFi password will be expected to be sent encrypted to the device.n

The encryption keys are SSID and the Konker password  of the device. To understand better please open the file _konker.h_ and verify the `getWifiCredentialsEncripted` function

If set false, the password has to be sent in plain text.

- `<http server>` : Domain / IP of the Konker platform

> Example: (char*)"data.demo.konkerlabs.net"

- `<http port>`: Port of the platform used for HTTP connections

> Example:  (int)80

Example:

    void setup(){
        Serial.begin(115200);
        Serial.println("Setup");

        konkerConfig((char*)"data.demo.konkerlabs.net:1883",(char*)"S0101",false,(char*)"data.demo.konkerlabs.net", 80);

        Serial.println("Setup finished");
    }

After the setup in the main loop put:

    konkerLoop();

Example:

    void loop(){
        konkerLoop();
    }

You can now use the following functions to publish and subscribe to a MQTT channel (remember to include _konkerMQTT.h_ to use them):

- `pubHTTP(<channel>, <message>)`
- `pubMQTT(<channel>, <message>)`

Example:

    const int capacity = JSON_OBJECT_SIZE(1024);
    StaticJsonDocument<capacity> jsonMSG;

    delay(10);

    jsonMSG["deviceId"] = (String)getChipId();
    jsonMSG["p"] = presenceCount;

    char bufferJson[1024];
    serializeJson(jsonMSG, bufferJson);;

    if(!pubMQTT(status_channel, bufferJson)){
        Serial.println("Message published");
    }else{
        Serial.println("Failed to publish message");
    }

- `subHTTP(<channel>, <callback function for this channel>)` -> subscriptions in HTTP are a GET request. To verify if the value in the channel has changed you have to poll it.

- `subHTTP()` usually has to be in a loop or be a scheduled function.

- `subMQTT(<channel>, <callback function for this channel>)` -> to use MQTT functions you have to include _konkerMQTT.h_. Since MQTT subscriptions stays listening to the channel, no polling is needed.

The callback function will be automatically called when some new value arrives at the channel.

The callback function must have this format:

    void function_name(byte* payload, unsigned int length)

# Device credentials and setup

PROCEDURE

1 - THE DEVICE HAS TO BE CONFIGURED AT FACTORY WITH THE KONKER PLATFORM CREDENTIALS

The device will search for

- wifi: KonkerDevNetwork

- password: konkerkonker123

When connected, the device will search for platform credentials file in the root folder of the device

> Example: \S010113610232

THE FILE HAS THIS FORMAT:

> {"srv":"mqtt.demo.konkerlabs.net","prt":"1883","usr":"jnu56qt1bb1i","pwd":"3S7usR9g5K","prx":"data"}

2 - IF NOT POSSIBLE TO CONNECT TO WIFI. THE DEVICE WILL REBOOT, AND CREATE A HOTSPOT WITH ITS NAME

> Example: S010113610232

CONNECT TO THE HOTSPOT AND MAKE A GET REQUEST LIKE THE EXAMPLE BELOW TO SEND THE WIFI

__Credentials__

   You could save up to 3 different wifi credentials for 3 different wifi

   - http://192.168.4.1/wifisave?s0=SSID_NAME&p0=ENCRIPTED_SSID_PASSWORD

More than one credential will be

   - http://192.168.4.1/wifisave?s0=SSID_NAME1&p0=ENCRIPTED_SSID_PASSWORD1&s1=SSID_NAME2&p1=ENCRIPTED_SSID_PASSWORD2

OR IF konkerConfig encryption flag is off

   - http://192.168.4.1/wifisave?s0=SSID_NAME&p0=SSID_PASSWORD

# Firmware updates

- For firmware updates check just call the function:

    checkForUpdates();

- Tip: don't leave the execution of `checkForUpdates()` in the main loop.  Call this function hourly or daily for example.
