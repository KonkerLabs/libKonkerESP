#ifndef __CREDENTIALSCLASS_H__
#define __CREDENTIALSCLASS_H__


#include <EEPROM.h>


#define MAX_NUM_WIFI_CRED         3
#define CRED_WIFI_ARRAY_SIZE     16
#define CRED_PLAT_ARRAY_SIZE     24


class CredentialsClass
{
    public:
        CredentialsClass();

        ~CredentialsClass();

        void begin();

        int storeWifiCredentials();
        int recoverWifiCredentials();
        int getWifiCredential(int pos, char * const ssid, char * const passwd, unsigned int size, char * const phy_mode);
        int setWifiCredential(int pos, char * const ssid, char * const passwd, char phy_mode);

        void printWiFiCredentials();

        int storePlatformCredentials();
        int recoverPlatformCredentials();
        int getPlatformCredential(char * const user, char * const passwd, unsigned int size);
        void setPlatformCredential(const char * user, const char * passwd);
        
        void printPlatformCredentials();

        void setExample();

    private:

        EEPROMClass mem_cred;

        struct wifi_cred_st_t
        {
            uint16_t enabled;
            char ssid[CRED_WIFI_ARRAY_SIZE];
            char passwd[CRED_WIFI_ARRAY_SIZE];
            char phy_mode;
        };

        wifi_cred_st_t wifi_cred[MAX_NUM_WIFI_CRED];

        struct plat_cred_st_t
        {
            uint16_t enabled;
            char user[CRED_PLAT_ARRAY_SIZE];
            char passwd[CRED_PLAT_ARRAY_SIZE];
        };

        plat_cred_st_t plat_cred;

        uint16_t crc_16 (uint8_t *buffer, unsigned int length);

        void clearWiFiCredential(wifi_cred_st_t & wifi_credential);

        void clearPlatformCredential();
};


#endif /* __CREDENTIALSCLASS_H__ */
