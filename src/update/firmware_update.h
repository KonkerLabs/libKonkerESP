/*
  Firmware update class. Part of Konker FW.
  @author Maria Júlia Berriel de Sousa
*/
#ifndef _FW_UPDATE_H_
#define _FW_UPDATE_H_

#include "firmware_update_definitions.h"

class ESPHTTPKonkerUpdate: public ESP8266HTTPUpdate
{
private:
  Protocol * _httpProtocol;
  String _manifestEndpoint;
  String _fwEndpoint;
  State _deviceState;
  unsigned long _last_time_update_check;

  fw_info_t currentFwInfo;
  ManifestHandler * manifest;

  // returns true if there is a new manifest
  int querryPlatform();
  bool validateUpdate();

  void prepareUpdate();
  bool finalizingSteps();

  void sendStatusMessage(int msgIndex);
  void sendExceptionMessage(int exptIndex);
  void sendUpdateConfirmation();

public:
  ESPHTTPKonkerUpdate(Protocol *client, String * manifestEndpoint);
  ESPHTTPKonkerUpdate();
  ~ESPHTTPKonkerUpdate();

  void setProtocol(Protocol *client);
  void setDeviceId(const char * id);
  // void setFWchannel(String id);

  bool checkForUpdate();
  void performUpdate();
  //To be called during setup
  bool checkFirstBoot();

  // Called by atUpdateEnd callback
  static void sendFwReceivedMessage();
};

#endif /* _FW_UPDATE_H_ */
