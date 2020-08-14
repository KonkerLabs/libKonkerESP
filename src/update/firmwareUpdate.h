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

enum StatusMessages
{
  MSG_MANIFEST_RECEIVED = 0,
  MSG_MANIFEST_CORRECT,
  MSG_FIRMEWARE_RECEIVED,
  MSG_CHECKSUM_OK,
  MSG_UPDATE_DONE,
  MSG_UPDATE_CORRECT
};

enum ExceptionMessages
{
  EXPT_COULD_NOT_GET_MAN = 0,
  EXPT_MANIFEST_INCORRECT,
  EXPT_FW_NOT_FOUND,
  EXPT_CHECKSUM_NOK,
  EXPT_INCORRECT_START
};

// Usar?
// void onStart(HTTPUpdateStartCB cbOnStart)          { _cbStart = cbOnStart; }
// void onEnd(HTTPUpdateEndCB cbOnEnd)                { _cbEnd = cbOnEnd; }
// void onError(HTTPUpdateErrorCB cbOnError)          { _cbError = cbOnError; }
// void onProgress(HTTPUpdateProgressCB cbOnProgress) { _cbProgress = cbOnProgress; }
// using HTTPUpdateStartCB = std::function<void()>;
// using HTTPUpdateEndCB = std::function<void()>;
// using HTTPUpdateErrorCB = std::function<void(int)>;
// using HTTPUpdateProgressCB = std::function<void(int, int)>;

class ESPHTTPKonkerUpdate: public ESP8266HTTPUpdate
{
private:
  Protocol * _httpProtocol;
  String _fwEndpoint;
  String _manifestEndpoint;
  String _currentVersion;
  String _newVersion;
  State _deviceState;
  unsigned long _last_time_update_check;

  ManifestHandler * manifest;

  Protocol *getClient();
  // returns true if there is a new manifest
  int querryPlatform();
  bool validateUpdate();

  t_httpUpdate_return update(String newVersion);
  void updateSucessCallBack(const char newVersion[16]);
  void runUpdate(UPDATE_SUCCESS_CALLBACK_SIGNATURE);

  int downloadFirmware();
  int applyFirmware();

  void sendStatusMessage(int msgIndex);
  void sendUpdateConfirmation(String newVersion);
  void sendExceptionMessage(int exptIndex);

public:
  ESPHTTPKonkerUpdate(Protocol *client, String currentVersion);
  ESPHTTPKonkerUpdate();
  ~ESPHTTPKonkerUpdate();

  void setProtocol(Protocol *client);

  void setVersion(String version);
  String getVersion();
  void setFWchannel(String id);

  bool checkForUpdate();
  void performUpdate();
  //To be called during setup
  bool checkFirstBoot();
};

#endif /* _FW_UPDATE_H_ */
