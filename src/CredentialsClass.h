#ifndef __CREDENTIALSCLASS_H__
#define __CREDENTIALSCLASS_H__


#include <EEPROM.h>


#define MAX_NUM_WIFI_CRED   3

class CredentialsClass
{
    public:
        CredentialsClass();

        ~CredentialsClass();

        int saveWifiCredentials();
        int recoverWifiCredentials();

    private:

        EEPROMClass mem_cred;

        struct wifi_cred_st_t
        {
            uint32_t enabled;
            char ssid[16];
            char passwd[16];
        };

        wifi_cred_st_t wifi_cred[MAX_NUM_WIFI_CRED];

        struct plat_cred_st_t
        {
            uint32_t enabled;
            char user[16];
            char passwd[16];
        };

        plat_cred_st_t plat_cred;
};


#endif /* __CREDENTIALSCLASS_H__ */
