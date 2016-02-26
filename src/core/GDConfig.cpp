
#include "GDConfig.h"

GDConfig::GDConfig()
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
    uint8_t *p = reinterpret_cast<uint8_t*>(&m_data);

    // read
    for (uint8_t i = 0; i < sizeof(GD_DATA); )
    {
        *p++ = EEPROM.read(i++);
    }

    // is vesion nok and reset config
    if (m_data.m_version != GPSDOG_CONF_VERSION) {
        this->cleanConfig();
    }
}

void GDConfig::writeConfig()
{
    uint8_t *p = reinterpret_cast<uint8_t*>(&m_data);

    // write
    for (uint8_t i = 0; i < sizeof(GD_DATA); )
    {
        EEPROM.update(i++, *p++);
    }
}

void GDConfig::cleanConfig()
{
    /* Def values */
    m_data.m_version        = GPSDOG_CONF_VERSION;
    m_data.m_alarmInterval  = GPSDOG_CONF_ALARM_INTERVAL;

    // state
    m_data.m_isInit         = false;
    m_data.m_isWatch        = false;
    m_data.m_isAlarm        = false;
    m_data.m_isProtect      = false;
    m_data.m_isForward      = false;

    // options
    m_data.m_alarmNumbers   ^= m_data.m_alarmNumbers;
    m_data.m_forwardIdx     ^= m_data.m_forwardIdx;

    // sign
    memset(m_data.m_signNums, 0x00, GPSDOG_CONF_NUMBER_STORE);

    // phone store
    memset(m_data.m_number1, 0x00, (GPSDOG_CONF_NUM_SIZE +1) * GPSDOG_CONF_NUMBER_STORE);

    // pw
    memset(m_data.m_password, 0x00, GPSDOG_CONF_PW_SIZE +1);

    // GPS Data
    m_data.m_latitude   = 0;
    m_data.m_longitude  = 0;
}

bool GDConfig::setStoreNumber(uint8_t numStoreIdx, char *num, uint8_t sign)
{
    // index secure & sign not lager than num
    if (strlen(num) > GPSDOG_CONF_NUM_SIZE || numStoreIdx >= GPSDOG_CONF_NUMBER_STORE || numStoreIdx < 0 || strlen(num) <= sign) {
        return false;
    }

    // clear & copy
    memset(m_numbers[numStoreIdx], 0x00, GPSDOG_CONF_NUM_SIZE +1);
    strncpy(m_numbers[numStoreIdx], num, GPSDOG_CONF_NUM_SIZE);

    m_data.m_signNums[numStoreIdx] = sign;

    return true;
}

bool GDConfig::checkStoreNumber(uint8_t numStoreIdx, char *num)
{
    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE || numStoreIdx < 0 || num == NULL) {
        return false;
    }

    // check if NULL
    if (m_numbers[numStoreIdx][m_data.m_signNums[numStoreIdx]] == 0x00 || num[0] == 0x00) {
        return false;
    }

    // check number
    if (strstr(num, m_numbers[numStoreIdx] + m_data.m_signNums[numStoreIdx]) != NULL) {
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

bool GDConfig::addNumberWithNotify(uint8_t numStoreIdx, char *num, uint8_t sign, bool notify)
{
    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE || numStoreIdx < 0) {
        return false;
    }

    // set number to store
    if (!this->setStoreNumber(numStoreIdx, num, sign)) {
        return false;
    }

    // set Notify flag
    this->setAlarmNotify(numStoreIdx, notify);

    return true;
}

uint8_t GDConfig::getSignNumber(uint8_t numStoreIdx)
{
    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE || numStoreIdx < 0) {
        return false;
    }

    return m_data.m_signNums[numStoreIdx];
}

bool GDConfig::isAlarmNotifyOn(uint8_t numStoreIdx)
{
    // index secure
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE || numStoreIdx < 0) {
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
    if (numStoreIdx >= GPSDOG_CONF_NUMBER_STORE || numStoreIdx < 0) {
        return;
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
    switch (mode)
    {
        case GPSDOG_MODE_INIT       : return m_data.m_isInit;
        case GPSDOG_MODE_WATCH      : return m_data.m_isWatch;
        case GPSDOG_MODE_ALARM      : return m_data.m_isAlarm;
        case GPSDOG_MODE_PROTECT    : return m_data.m_isProtect;
        case GPSDOG_MODE_FORWARD    : return m_data.m_isForward;
    }

    return false;
}

void GDConfig::setMode(uint8_t mode, bool onOff)
{
    switch (mode)
    {
        case GPSDOG_MODE_INIT       : 
            m_data.m_isInit = onOff;
            return;
        case GPSDOG_MODE_WATCH      : 
            m_data.m_isWatch = onOff;
            return;
        case GPSDOG_MODE_ALARM      : 
            m_data.m_isAlarm = onOff;
            return;
        case GPSDOG_MODE_PROTECT    : 
            m_data.m_isProtect = onOff;
            return;
        case GPSDOG_MODE_FORWARD    : 
            m_data.m_isForward = onOff;
            return;
    }
}

void GDConfig::setForwardIdx(uint8_t val)
{
    // index secure
    if (val >= GPSDOG_CONF_NUMBER_STORE || val < 0) {
        return;
    }

    m_data.m_forwardIdx = val;
}

// vim: set sts=4 sw=4 ts=4 et:
