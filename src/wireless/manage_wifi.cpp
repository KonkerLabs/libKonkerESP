#include "manage_wifi.h"

WifiManager::WifiManager()
{
  WiFi.persistent(false);
  WiFi.disconnect();
  delay(10);
  WiFi.setAutoConnect(false);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_STA);
  delay(10);
  WiFi.setAutoReconnect(true);
}

String getConnectMessage(int status_code)
{
	switch(status_code)
  {
		case 255: return "WL_NO_SHIELD"; break;
		case 0: return "WL_IDLE_STATUS"; break;
		case 1: return "WL_NO_SSID_AVAIL"; break;
		case 2: return "WL_SCAN_COMPLETED"; break;
		case 3: return "WL_CONNECTED"; break;
		case 4: return "WL_CONNECT_FAILED"; break;
		case 5: return "WL_CONNECTION_LOST"; break;
		case 6: return "WL_DISCONNECTED"; break;
	}
	return "UNDEFINED";
}

bool WifiManager::tryConnect(String ssid, String password)
{
  int counter = 100;

	WiFi.mode(WIFI_OFF);
	delay(500);
	WiFi.mode(WIFI_STA);
	delay(500);
	//check if we have ssid and pass and force those, if not, try with last saved values
  Log.notice("[WiFi] WiFi.begin(%s, %s)\n", ssid.c_str(), password.c_str());
	WiFi.begin(ssid, password);

  // unsigned long wifiStartTime=millis();
	// Serial.println("checking for connection status");
	// Serial.print("wifi timeout = ");
	// Serial.println(__wifiTimout);
	// Serial.print("startTime = ");
	// Serial.println(wifiStartTime);
	// Serial.print("now = ");
	// Serial.println(millis());
  // NO WHILE (millis()-wifiStartTime)<(__wifiTimout*i)

  while (WiFi.status() != WL_CONNECTED && counter > 0)
  {
    delay(500);
    if (DEBUG_LEVEL>0) Serial.print(".");
    counter -= 1;
  }
  if (DEBUG_LEVEL>0) Serial.print("\n");

  int connRes = WiFi.status();
  Log.notice("[WiFi] Connection Status = %d %s\n", connRes, getConnectMessage(connRes).c_str());

  return (connRes == WL_CONNECTED);
}

bool WifiManager::tryConnectSSID(String ssid, String password, int retries)
{
  bool connRes = false;
  Log.notice("[WiFi] Will try to connect %d times\n", retries);
  if (DEBUG_LEVEL>0)
  {
    Serial.print(">>>>>> DEVICE MAC ADDRESS = ");
  	Serial.println(WiFi.macAddress());
  }
  for(int i=0; i < retries; i++)
  {
    connRes = this->tryConnect(ssid, password);
    if(connRes) break;
  }

  return connRes;
}

void WifiManager::setConfig(String ssid, String password)
{
  int index = 0, size;

  // if numWifiCredentials == 0 or numWifiCredentials == MAX_NUM_WIFI_CRED
  //   new credential is stored at position 0 (may overwrite)
  if(this->numWifiCredentials > 0 &&  this->numWifiCredentials < MAX_NUM_WIFI_CRED)
  {
    index = this->numWifiCredentials + 1;
  }

  Log.notice("[WiFi] Setting new WiFi credentials at index %d\n", index);

  size = ssid.length();
  strncpy(wifiCredentials[index].SSID, ssid.c_str(), size);
  this->wifiCredentials[index].SSID[size] = '\0';
  size = password.length();
  strncpy(wifiCredentials[index].PASSWD, password.c_str(), size);
  this->wifiCredentials[index].PASSWD[size] = '\0';
  this->wifiCredentials[index].enabled = ENABLED_PATTERN;

  if(this->numWifiCredentials < MAX_NUM_WIFI_CRED)
  {
    this->numWifiCredentials += 1;
  }
}

void WifiManager::setConfig(String ssid0, String password0, String ssid1, String password1)
{
  setConfig(ssid0, password0);
  setConfig(ssid1, password1);
}

void WifiManager::setConfig(String ssid0, String password0, String ssid1, String password1, String ssid2, String password2)
{
  setConfig(ssid0, password0);
  setConfig(ssid1, password1);
  setConfig(ssid2, password2);
}

void WifiManager::removeConfig(String ssid)
{
  wifi_credentials temp[MAX_NUM_WIFI_CRED];
  int numTemp = 0;

  for (int i=0; i<MAX_NUM_WIFI_CRED; i++) temp[i].enabled = 0x0000u;

  // copy credentials that do NOT match ssid to temp
  for(int i=0; i < this->numWifiCredentials; i++)
  {
    if(strncmp(this->wifiCredentials[i].SSID, ssid.c_str(), ssid.length()) == 0)
    {
      continue;
    }
    temp[numTemp].enabled = this->wifiCredentials[i].enabled;
    strncpy(temp[numTemp].SSID, this->wifiCredentials[i].SSID, WIFI_CRED_ARRAY_SIZE);
    strncpy(temp[numTemp].PASSWD, this->wifiCredentials[i].PASSWD, WIFI_CRED_ARRAY_SIZE);
    numTemp++;
  }

  if(numTemp == MAX_NUM_WIFI_CRED)
  {
    Log.trace("[WiFi] \"%s\" ssid not found in current credentials\n", ssid.c_str());
    return;
  }

  if(numTemp == 0)
  {
    Log.trace("[WiFi] No WiFi credentials left, please set it before connecting\n");
  }

  memcpy(this->wifiCredentials, temp, sizeof(this->wifiCredentials));
  this->numWifiCredentials = numTemp;
}

bool WifiManager::tryConnectClientWifi()
{
  bool conn = false;

  if(this->numWifiCredentials == 0)
  {
    Log.trace("[WiFi] Credentials are missing!\n");
    Log.trace("[WiFi] Trying to restore from EEPROM\n");
    if(!restoreWifiCredentials())
    {
      Log.warning("[WiFi] Credentials not found! Aborting\n");
      this->numConnFail++;
      return conn;
    }
  }

  for (int i=0; i < this->numWifiCredentials; i++)
  {
    Log.notice("[WiFi] Trying to connect to SSID: %s\n", this->wifiCredentials[i].SSID);

    conn = this->tryConnectSSID(String(this->wifiCredentials[i].SSID), String(this->wifiCredentials[i].PASSWD), 2);
  }

  if(!conn) this->numConnFail++;

  return conn;
}

bool WifiManager::checkConnectionStatus()
{
  return (WiFi.status() == WL_CONNECTED);
}

void WifiManager::disconnectClientWifi()
{
  WiFi.mode(WIFI_OFF);
  delay(100);
}

int WifiManager::getNumberOfWifiCredentials()
{
  return this->numWifiCredentials;
}

IPAddress WifiManager::getLocalIP()
{
  IPAddress ip;

  ip = WiFi.localIP();
  // Log.trace("Local IP address: %s\n", ip.toString().c_str());

  return ip;
}

int WifiManager::getWifiStrenght()
{
  return WiFi.RSSI();
}

String WifiManager::getWifiSSID()
{
  return WiFi.SSID();
}

String WifiManager::getMacAddress()
{
  byte mac[6];
  char macString[20];
  WiFi.macAddress(mac);
  //  char macString[20];
  sprintf(macString, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4],mac[5]);

  return String(macString);
}

int WifiManager::saveWifiCredentials()
{
  uint8_t buffer[MAX_NUM_WIFI_CRED * sizeof(wifi_credentials)];

  memset(buffer, 0, sizeof(buffer));
  Log.trace("[WiFi] Saving WiFi credentials to EEPROM\n");

  for (int i=0; i< this->numWifiCredentials; i++)
  {
    Log.trace("[WiFi] Saving %s<>%s @ %d\n", this->wifiCredentials[i].SSID, this->wifiCredentials[i].PASSWD, i * sizeof(wifi_credentials));
    memcpy(&buffer[i * sizeof(wifi_credentials)], &this->wifiCredentials[i], sizeof(wifi_credentials));
  }

  return deviceEEPROM.storeWifiCredentials(buffer);
}

int WifiManager::restoreWifiCredentials()
{
  uint8_t retBuffer[MAX_NUM_WIFI_CRED * sizeof(wifi_credentials)];
  wifi_credentials temp_cred;
  int ret, numWifiTemp = 0;

  memset(retBuffer, 0, sizeof(retBuffer));
  ret = deviceEEPROM.recoverWifiCredentials(retBuffer);

  if (ret)
  {
    for (int i=0; i< MAX_NUM_WIFI_CRED; i++)
    {
      memcpy(&temp_cred, &retBuffer[i * sizeof(wifi_credentials)], sizeof(wifi_credentials));

      if(temp_cred.enabled == ENABLED_PATTERN)
      {
        Log.trace("[WiFi] Restored credentials: %s<>%s\n", temp_cred.SSID, temp_cred.PASSWD);
        setConfig(String(temp_cred.SSID), String(temp_cred.PASSWD));
        numWifiTemp++;
      }
    }
    this->numWifiCredentials = numWifiTemp;
  }
  else
  {
    Log.warning("[WiFi] Could not restore WiFi credentials from memory!\n");
  }

  return ret;
}
