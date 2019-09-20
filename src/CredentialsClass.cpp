#include "CredentialsClass.h"
#include <EEPROM.h>
#include <Arduino.h>

#define ENABLED_PATTERN   0XABCD


CredentialsClass::CredentialsClass()
{
    mem_cred.begin(4096);
    yield();
}

CredentialsClass::~CredentialsClass()
{
    // nothing here
}

int CredentialsClass::saveWifiCredentials()
{
    uint8_t * dataptr = (uint8_t *)&wifi_cred[0];

    for (int i = 0; i < sizeof(wifi_cred); ++i)
    {
        mem_cred.write(i, *dataptr);
    }

    return mem_cred.commit() ? 0 : -1;
}
