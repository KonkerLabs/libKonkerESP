
#ifndef __KONKER_H__
#define __KONKER_H__

#ifndef ESP32
extern "C" {
  #include "user_interface.h"
}
#endif

#include <Arduino.h>

#ifndef ESP32
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#endif

#include "config_wifi.h"
#include "globals.h"
#include "./protocols/protocol.h"
#include "buffer_entry.h"
#include "./update/firmwareUpdate.h"

// debug levels
/*
// 0 - OFF	The lowest possible rank and is intended to turn off logging.
// 1 - INFO	Designates informational messages that highlight the progress of the application at coarse-grained level.
// 2 - DEBUG	Designates fine-grained informational events that are most useful to debug an application.
// 3 - WARN	Designates potentially harmful situations.
// 4 - ERROR	Designates error events that might still allow the application to continue running.
// 5 - FATAL	Designates very severe error events that will presumably lead the application to abort.
// 6 - ALL	All levels including custom levels.
*/

#define MAX_NUM_WIFI_CRED   3

// define WIFI credentials and manipulation
//
struct WifiCredentials
{
    String savedSSID;
    String savedPSK;
};

// Konker ESP CLASS

class KonkerDevice {

  private:

    WifiCredentials wifiCredentials[MAX_NUM_WIFI_CRED];
    int numWifiCredentials = 0;

    int wifi_connection_errors = 0;

    ConnectionType defaultConnectionType;
    ConnectionType fallbackConnectionType;

    #ifndef ESP32
    int resetPin = D5;
    #else
    int resetPin=13;
    #endif

    String wifiFile;

    bool _encripted=true;
    // WiFiServer httpServer(80);// create object
    #ifndef ESP32
    ESP8266WebServer webServer;
    #else
    WebServer webServer;
    #endif

    ESPHTTPKonkerUpdate update;

    // identificacao do device
    String ChipId;
    String id;

    String NAME = DEFAULT_NAME;

    // fila de envio do device
    BufferEntry sendBuffer[BUFFER_SIZE];
    BufferEntry receiveBuffer[BUFFER_SIZE];

    // flag when the device checked for a device update
    unsigned long _last_time_update_check=0;

    String _health_channel = "_health";

    void flushBuffer();

    void formatFileSystem();

    Protocol* currentProtocol;

    String host;
    int port;

    String userid;
    String password;

  public:

    KonkerDevice();
    ~KonkerDevice();

    // configuration methods

    void setServer(String host, int port);
    void setPlatformCredentials(String userid, String password);

    void addWifi(String ssid, String password);
    void clearWifi(String ssid);
    int getNumWifiCredentials();

    void setUniqueID(String id);
    String getUniqueID();

    void setDefaultConnectionType(ConnectionType c);
    void setFallbackConnectionType(ConnectionType c);
    ConnectionType getDefaultConnectionType();
    ConnectionType getFallbackConnectionType();

    void saveConfiguration();

    // operation methods

    void startConnection();
    void stopConnection();
    int checkConnection();

    void resetALL();

    void loop();

    // communication interface

    /* returns 1 if send is OK or <= 0 if error ocurred when sending data to the server */
    void send(String payload);
    void send(BufferEntry *data);

    /* returns 1 if a message exists and is copied to the buffer or 0 if nothing exists */
    int receive(String *buffer);

    HTTPClient* getAPIClient();

    // internal interface
    // heart beat to the server to send status information for the device
    void healthUpdate(String healthChannel);
    // check if is there any update / reconfiguration for this device on the platform
    void checkForUpdates();

    void setName(const char * newName);
};

//
// simple usage
//


// declare a KonkerDevice() object in your sketch
// in setup()
//    ... initialize / setup konker device with platform and wifi credentials
//    ... if desired, define the default and fallback protocol for the device
//
// in loop()
//    .. call device.loop() method
//    .. when you want to send data ... call device.send(payload)
//    .. when you need to receive a data, call device.receive() to get a payload if it exists
//
//

#endif
