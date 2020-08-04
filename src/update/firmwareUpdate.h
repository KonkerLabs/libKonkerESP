/*
  Firmware update class. Part of Konker FW.
  @author Maria Júlia Berriel de Sousa
*/
#ifndef _FW_UPDATE_H_
#define _FW_UPDATE_H_

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "globals.h"
#include "manifest_handler.h"
#include "../protocols/connection/http_protocol.h"

enum State {RUNNING, UPDATING, REBOOT, UPDATED};

// Usar?
// void onStart(HTTPUpdateStartCB cbOnStart)          { _cbStart = cbOnStart; }
// void onEnd(HTTPUpdateEndCB cbOnEnd)                { _cbEnd = cbOnEnd; }
// void onError(HTTPUpdateErrorCB cbOnError)          { _cbError = cbOnError; }
// void onProgress(HTTPUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }
// using HTTPUpdateStartCB = std::function<void()>;
// using HTTPUpdateEndCB = std::function<void()>;
// using HTTPUpdateErrorCB = std::function<void(int)>;
// using HTTPUpdateProgressCB = std::function<void(int, int)>;

class ESPHTTPKonkerUpdate: public ESP8266HTTPUpdate, ManifestHandler
{
private:
  Protocol * _httpProtocol;
  String _fwEndpoint;
  String _currentVersion;
  String _newVersion;
  State _deviceState;
  unsigned long _last_time_update_check;

  fw_info currentFwInfo;

  String getVersionFromPayload(String strPayload);
  void updateVersion(String newVersion);
  bool querryPlatform(String recvVersion);

  t_httpUpdate_return update(String newVersion);
  void updateSucessCallBack(const char newVersion[16]);
  void runUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE);

public:
  ESPHTTPKonkerUpdate(Protocol *client, String currentVersion);
  ESPHTTPKonkerUpdate();
  ~ESPHTTPKonkerUpdate();

  void setProtocol(Protocol *client);
  Protocol *getClient();
  void setVersion(String version);
  String getVersion();
  void setFWchannel(String id);

  bool checkForUpdate();
  bool validateUpdate();
  void performUpdate();
  //To be called during setup
  bool checkFirstBoot();
  void sendUpdateConfirmation(String newVersion);

  int downloadFirmware();
  int applyFirmware();

  int sendStatusMessage();
  int sendExceptionMessage();
};

#endif /* _FW_UPDATE_H_ */
