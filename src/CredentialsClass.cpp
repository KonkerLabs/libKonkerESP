#include "CredentialsClass.h"
#include <EEPROM.h>
#include <Arduino.h>

#define ENABLED_PATTERN       0xABCDu
#define PLATF_CRED_INIT_ADDR  (sizeof(wifi_cred) + 2)


CredentialsClass::CredentialsClass()
{
    // nothing here
}

CredentialsClass::~CredentialsClass()
{
    mem_cred.end();
}

void CredentialsClass::begin()
{
    mem_cred.begin(4096);
    yield();

    recoverWifiCredentials();
    recoverPlatformCredentials();
}

int CredentialsClass::storeWifiCredentials()
{
    uint8_t * dataptr = (uint8_t *)(void*)&wifi_cred[0];

    for (int i = 0; i < MAX_NUM_WIFI_CRED; ++i)
    {
        if (wifi_cred[i].enabled != ENABLED_PATTERN)
        {
            clearWiFiCredential(wifi_cred[i]);
        }
    }

    for (unsigned int i = 0; i < sizeof(wifi_cred); ++i)
    {
        mem_cred.write(i, *dataptr++);
    }

    uint16_t crc = crc_16((uint8_t *)(void*)&wifi_cred[0], sizeof(wifi_cred));
    Serial.printf("crc_store = %x\r\n", crc); // fixme
    mem_cred.write(sizeof(wifi_cred), crc >> 8);
    mem_cred.write(sizeof(wifi_cred)+1, crc & 0xFFu);

    return mem_cred.commit() ? 0 : -1;
}

int CredentialsClass::recoverWifiCredentials()
{
    int ret = 0;
    uint8_t * dataptr = (uint8_t *)(void*)&wifi_cred[0];

    for (unsigned int i = 0; i < sizeof(wifi_cred); ++i)
    {
        dataptr[i] = mem_cred.read(i);
    }

    uint16_t stored_crc = mem_cred.read(sizeof(wifi_cred)) << 8 | mem_cred.read(sizeof(wifi_cred)+1);
    Serial.printf("crc_stored = %x\r\n", stored_crc);
    Serial.println(crc_16((uint8_t *)(void*)&wifi_cred[0], sizeof(wifi_cred)), HEX);

    if (stored_crc != crc_16((uint8_t *)(void*)&wifi_cred[0], sizeof(wifi_cred)))
    {
        ret = -1;
    }
       
    return ret;
}

int CredentialsClass::getWifiCredential(int pos, char * const ssid, char * const passwd, unsigned int size, char * const phy_mode)
{
    int ret = 0;

    if ( (pos < 0) || (pos > (MAX_NUM_WIFI_CRED-1)) )
    {
        ret = -2;
    }
    else if (wifi_cred[pos].enabled == ENABLED_PATTERN)
    {
        strncpy(ssid, wifi_cred[pos].ssid, size);
        strncpy(passwd, wifi_cred[pos].passwd, size);
        *phy_mode = wifi_cred[pos].phy_mode;
    }

    return ret;
}

int CredentialsClass::setWifiCredential(int pos, char * const ssid, char * const passwd, char phy_mode)
{
    int ret = 0;

    if ( (pos < 0) || (pos > (MAX_NUM_WIFI_CRED-1)) )
    {
        ret = -2;
    }
    else
    {
        wifi_cred[pos].enabled = ENABLED_PATTERN;
        strncpy(wifi_cred[pos].ssid, ssid, CRED_WIFI_ARRAY_SIZE);
        strncpy(wifi_cred[pos].passwd, passwd, CRED_PLAT_ARRAY_SIZE);
        wifi_cred[pos].phy_mode = phy_mode;

        wifi_cred[pos].ssid[CRED_WIFI_ARRAY_SIZE-1] = '\0';
        wifi_cred[pos].passwd[CRED_WIFI_ARRAY_SIZE-1] = '\0';
    }

    return ret;
}

void CredentialsClass::printWiFiCredentials()
{
    for (int i = 0; i < MAX_NUM_WIFI_CRED; ++i)
    {
        if (wifi_cred[i].enabled == ENABLED_PATTERN)
        {
            Serial.printf("[%d]\r\n", i);
            Serial.printf("SSID: %s\r\n", wifi_cred[i].ssid);
            Serial.printf("PASSWD: %s\r\n", wifi_cred[i].passwd);
            Serial.printf("PHY Mode: %c\r\n\r\n", wifi_cred[i].phy_mode);
        }
    }
}

int CredentialsClass::storePlatformCredentials()
{
    uint8_t * dataptr = (uint8_t *)(void*)&plat_cred;

    if (plat_cred.enabled != ENABLED_PATTERN)
    {
        clearPlatformCredential();
    }

    for (unsigned int i = 0; i < sizeof(plat_cred); ++i)
    {
        mem_cred.write(PLATF_CRED_INIT_ADDR + i, *dataptr++);
    }

    uint16_t crc = crc_16((uint8_t *)(void*)&plat_cred, sizeof(plat_cred));
    Serial.printf("crc_store = %x\r\n", crc); // fixme
    mem_cred.write(PLATF_CRED_INIT_ADDR+sizeof(plat_cred), crc >> 8);
    mem_cred.write(PLATF_CRED_INIT_ADDR+sizeof(plat_cred)+1, crc & 0xFFu);

    return mem_cred.commit() ? 0 : -1;
}

int CredentialsClass::recoverPlatformCredentials()
{
    int ret = 0;
    uint8_t * dataptr = (uint8_t *)(void*)&plat_cred;

    for (unsigned int i = 0; i < sizeof(plat_cred); ++i)
    {
        dataptr[i] = mem_cred.read(PLATF_CRED_INIT_ADDR + i);
    }

    uint16_t stored_crc = mem_cred.read(PLATF_CRED_INIT_ADDR+sizeof(plat_cred)) << 8 | mem_cred.read(PLATF_CRED_INIT_ADDR+sizeof(plat_cred)+1);
    Serial.printf("crc_stored = %x\r\n", stored_crc);
    Serial.println(crc_16((uint8_t *)(void*)&plat_cred, sizeof(plat_cred)), HEX);

    if (stored_crc != crc_16((uint8_t *)(void*)&plat_cred, sizeof(plat_cred)))
    {
        ret = -1;
    }
       
    return ret;
}

int CredentialsClass::getPlatformCredential(char * const user, char * const passwd, unsigned int size)
{
    int ret = 0;

    if (plat_cred.enabled == ENABLED_PATTERN)
    {
        strncpy(user, plat_cred.user, size);
        strncpy(passwd, plat_cred.passwd, size);
    }
    else
    {
        ret = -1;
        user[0] = '\0';
        passwd[0] = '\0';
    }

    return ret;
}

void CredentialsClass::setPlatformCredential(const char * user, const char * passwd)
{
    plat_cred.enabled = ENABLED_PATTERN;
    strncpy(plat_cred.user, user, CRED_PLAT_ARRAY_SIZE);
    strncpy(plat_cred.passwd, passwd, CRED_PLAT_ARRAY_SIZE);

    plat_cred.user[CRED_PLAT_ARRAY_SIZE-1] = '\0';
    plat_cred.passwd[CRED_PLAT_ARRAY_SIZE-1] = '\0';
}

void CredentialsClass::printPlatformCredentials()
{
    if (plat_cred.enabled == ENABLED_PATTERN)
    {
        Serial.printf("user: %s\r\n", plat_cred.user);
        Serial.printf("passwd: %s\r\n\r\n", plat_cred.passwd);
    }
}

uint16_t CredentialsClass::crc_16 (uint8_t *buffer, unsigned int length)
{
    uint16_t temp_bit, temp_int, crc;
    crc = 0xFFFF;

    for (unsigned int i = 0; i < length; i++)
    {
        temp_int = (uint8_t) *buffer++;
        crc ^= temp_int;
        for (int j = 0; j < 8; j++)
        {
            temp_bit = crc & 0x0001;
            crc >>= 1;
            if (temp_bit != 0)
                crc ^= 0xA001;
        }
    }

    return crc;
}

void CredentialsClass::clearWiFiCredential(wifi_cred_st_t & wifi_credential)
{
    memset(&wifi_credential, 0xFFu, sizeof(wifi_cred[0]));
}

void CredentialsClass::clearPlatformCredential()
{
    memset(&plat_cred, 0xFFu, sizeof(plat_cred));
}

void CredentialsClass::setExample()
{
    // wifi_cred[0].enabled = ENABLED_PATTERN;
    // snprintf(wifi_cred[0].ssid, CRED_WIFI_ARRAY_SIZE-1, "1234");
    // snprintf(wifi_cred[0].passwd, CRED_WIFI_ARRAY_SIZE-1, "1234konker");
    // wifi_cred[0].phy_mode = 'a';

    wifi_cred[1].enabled = 0;
    snprintf(wifi_cred[1].ssid, CRED_WIFI_ARRAY_SIZE-1, "erickmf");
    snprintf(wifi_cred[1].passwd, CRED_WIFI_ARRAY_SIZE-1, "erickmf");
    wifi_cred[1].phy_mode = 'g';

    // wifi_cred[2].enabled = ENABLED_PATTERN;
    // snprintf(wifi_cred[2].ssid, CRED_WIFI_ARRAY_SIZE-1, "konkerbeg");
    // snprintf(wifi_cred[2].passwd, CRED_WIFI_ARRAY_SIZE-1, "12345678");
    // wifi_cred[2].phy_mode = 'n';

    // plat_cred.enabled = ENABLED_PATTERN;
    // snprintf(plat_cred.user, CRED_PLAT_ARRAY_SIZE-1, "usuario_bobo");
    // snprintf(plat_cred.passwd, CRED_PLAT_ARRAY_SIZE-1, "senha1234");
}
