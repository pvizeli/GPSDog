
#include "GDConfig.h"

void GDConfig::GDConfig()
{
    this->readConfig();

    // prepare number array
    for (uint8_t i = 0; i < GPSDOG_CONF_NUMBER_STORE; ) 
    {
        // calc adress in memory
        m_numbers[i++] = m_data.m_number1 + i * (GPSDOG_CONF_NUM_SIZE + 1);
    }
}

void GDConfig::readConfig()
{
    byte *p = reinterpret_cast<byte*>(&m_data);

    // read
    for (uint8_t i = 0; i < sizeof(GD_DATA); )
    {
        *p++ = EEPROM.read(i++);
    }

    // is vesion nok and reset config
    if (m_data.m_version != GPSDOG_CONF_VERSION) {
        this->cleanConfig();
    }
    else {
        // reset Mode for only save init
        m_data.m_mode &= GPSDOG_MODE_INIT;
    }
}

void GDConfig::writeConfig()
{
    byte *p = reinterpret_cast<byte*>(&m_data);

    // write
    for (uint8_t i = 0; i < sizeof(GD_DATA); )
    {
        EEPROM.write(i++, *p++);
    }
}

void GDConfig::cleanConfig()
{
    m_data.m_version = GPSDOG_CONF_VERSION;

    // options
    m_data.m_mode           ^= m_data.m_mode;
    m_data.m_alarmNumbers   ^= m_data.m_alarmNumbers;

    // phone store
    memset(m_data.m_number1, 0x00, GPSDOG_CONF_NUM_SIZE +1);
    memset(m_data.m_number2, 0x00, GPSDOG_CONF_NUM_SIZE +1);
    memset(m_data.m_number3, 0x00, GPSDOG_CONF_NUM_SIZE +1);
    memset(m_data.m_number4, 0x00, GPSDOG_CONF_NUM_SIZE +1);

    // pw
    memset(m_data.m_password, 0x00, GPSDOG_CONF_PW_SIZE +1);
}

bool GDConfig::setStoreNumber(uint8_t numStoreIdx, char *num)
{
    // index secure
    if (strlen(num) > GPSDOG_CONF_NUM_SIZE || numStoreIdx >= GPSDOG_CONF_NUMBER_STORE) {
        return false;
    }

    // clear
    memset(m_numbers[numStoreIdx], 0x00, GPSDOG_CONF_NUM_SIZE +1);
   
    // copy
    strncpy(m_numbers[numStoreIdx], num, GPSDOG_CONF_NUM_SIZE);

    return true;
}

bool GDConfig::checkStoreNumber(uint8_t numStoreIdx, char *num)
{
    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE) {
        return false;
    }

    // check number
    if (strstr(m_numbers[numStoreIdx], num, GPSDOG_CONF_NUM_SIZE) != NULL) {
        return true;
    }

    return false;
}

bool GDConfig::foundNumberInStore(char *num)
{
    // search in store numbers 
    for (uint8_t i = 0; i < GPSDOG_CONF_NUMBER_STORE; i++) {

        // compare number
        if (this->checkStoreNumber(i, num)) {
            return true;
        }
    }

    // not found
    return false;
}


bool GDConfig::isAlarmNotifyOn(uint8_t numStoreIdx)
{
    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE) {
        return false;
    }

    if (m_data.m_alarmNumbers & (numStoreIdx*2) != 0x00) {
        return true;
    }

    return false;
}

void GDConfig::setAlarmNotify(uint8_t numStoreIdx, bool onOff)
{
    uint8_t mask = numStoreIdx * 2;

    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE) {
        return false;
    }

    if (onOff) {
        // ON
        m_data.m_alarmNumbers |= mask;
    }
    else {
        // OFF
        m_data.m_alarmNumbers ^= mask; 
    }
}

bool GDConfig::setPasswordInit(char *pw)
{
    // Size to big or pw is already set
    if (strlen(pw) > GPSDOG_CONF_PW_SIZE || this->isModeOn(GPSDOG_MODE_INIT)) {
        return false;
    }

    // copy data / set init flag
    strncpy(m_data.m_password, pw, GPSDOG_CONF_PW_SIZE);
    this->setMode(GPSDOG_MODE_INIT, true);

    return true;
}

bool GDConfig::checkPassword(char *pw)
{
    if (strncmp(m_data.m_password, pw, GPSDOG_CONF_PW_SIZE) == 0) {
        return true;
    }

    return false;
}

bool GDConfig::isModeOn(uint8_t mode)
{
    if ((m_data.m_mode & mode) == mode) {
        return true;
    }

    return false;
}

void GDConfig::setMode(uint8_t mode, bool onOff)
{
    if (onOff) {
        // ON
        m_data.m_mode |= mode;
    }
    else {
        // OFF
        m_data.m_mode ^= mode; 
    }
}

// vim: set sts=4 sw=4 ts=4 et:
