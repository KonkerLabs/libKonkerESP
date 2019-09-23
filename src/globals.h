#ifndef __GLOBALS_H__
#define __GLOBALS_H__

//#define UPDATE_SUCCESS_CALLBACK_SIGNATURE void (*succes_update_callback)(char[16])
#define UPDATE_SUCCESS_CALLBACK_SIGNATURE void (ESPHTTPKonkerUpdate::* updateSucessCallBack_t)(const char[16])

#define DEFAULT_NAME    "S000"
#define BUFFER_SIZE      1000
#define DEBUG_LEVEL      0

#ifndef BUILD_ID
#define BUILD_ID "DEFAULT_VERSION"
#endif

// connection status
#define NOT_CONNECTED 0
#define CONNECTED 1
#define DISCONNECTED 2

#endif /* __GLOBALS_H__ */
