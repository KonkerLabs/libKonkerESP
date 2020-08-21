#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <ArduinoLog.h>

#define _STATUS_LED 2

//#define UPDATE_SUCCESS_CALLBACK_SIGNATURE void (*succes_update_callback)(char[16])
#define UPDATE_SUCCESS_CALLBACK_SIGNATURE void (ESPHTTPKonkerUpdate::* updateSucessCallBack_t)(const char[16])

#define DEFAULT_NAME      "S000"
#define BUFFER_SIZE       10

// if this in larger than 2^16, health_conn_st_t will need to be adjusted
#define MAX_CONN_FAIL     1000

enum class ConnectionType
{
  HTTP, MQTT, AMQP, TCP, UDP, HTTPS, MQTTS
};

// debug levels
/*
* 0 - LOG_LEVEL_SILENT     no output
* 1 - LOG_LEVEL_FATAL      fatal errors
* 2 - LOG_LEVEL_ERROR      all errors
* 3 - LOG_LEVEL_WARNING    errors, and warnings
* 4 - LOG_LEVEL_NOTICE     errors, warnings and notices
* 5 - LOG_LEVEL_TRACE      errors, warnings, notices & traces
* 6 - LOG_LEVEL_VERBOSE    all

Obs: To fully remove all logging code, uncomment #define DISABLE_LOGGING in ArduinoLog.h, this may significantly reduce your sketch/library size.
*/
#define DEBUG_LEVEL         6

#ifndef BUILD_ID
#define BUILD_ID            "DEFAULT_VER"
#endif

// connection status
#define NOT_CONNECTED       0
#define CONNECTED           1
#define DISCONNECTED        2
#define UNKNOWN             3

#endif /* __GLOBALS_H__ */
