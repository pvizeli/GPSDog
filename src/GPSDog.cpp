
// includes
#include "GPSDog.h"


GPSDog::GPSDog()
{
    m_isInit        = false;
    m_nextAlarmSMS  ^= m_nextAlarmSMS;
}
        
void GPSDog::initialize(char *smsNum, uint8_t smsNumSize, char *smsTxt, uint8_t smsTxtSize, bool (*cbSendSMS)(), void (*cbCheckSMS)(), void (*cbReceiveGPS)())
{
    // init data
    m_number        = smsNum;
    m_numberSize    = smsNumSize;
    m_message       = smsTxt;
    m_messageSize   = smsTxtSize;

    // callbacks
    cb_sendSMS          = cbSendSMS;
    cb_checkNewSMS      = cbCheckSMS;
    cb_receiveGPS       = cbReceiveGPS;

    // set init flag
    m_isInit        = true;
}

void GPSDog::mainProcessing()
{
    // check is init
    if (!m_isInit) {
        return;
    }

    while (1) {

        ////
        // if Alarm mode is on
        if (this->isModeOn(GPSDOG_MODE_ALARM)) {
            // Time to new send a alarm
            if (m_nextAlarmSMS == millis()) {
                this->sendAlarmSMS();
            }
        }

        // process command sms
        this->cb_checkNewSMS;

        // processing GPS data
        this->cb_receiveGPS;

        // wait for next process
        delay(GPSDOG_WAIT_PROCESSING);
    }
}

void GPSDog::processIncomingSMS()
{
    char    *smsCmd;
    uint8_t count;
    bool    legalNum;

    // If Protect mode active
    legalNum = this->foundNumberInStore(m_number);

    ////
    // Parse Data
    count   = this->parseSMSMessage();
    smsCmd  = this->getSMSCommand();

    ////
    // Find Master command

    // STATUS
    if (strncmp_P(smsCmd, GPSDOG_TXT_STATUS, 6) && count == 0) {
        // check modus for legal number or protected is off
        if (legalNum || !this->isModeOn(GPSDOG_MODE_PROTECT)) {
            this->createStatusSMS();
        }
    }
    // INIT pw number ON/OFF
    else if (strncmp_P(smsCmd, GPSDOG_TXT_INIT, 4) && count == 3) {
        this->readInitFromSMS();
    }
    // RESET pw
    else if (strncmp_P(smsCmd, GPSDOG_TXT_RESET, 5) && count == 1) {
        this->readResetFromSMS();
    }
    // STORE num ADD number ON/OFF
    // STORE num DEL
    // STORE num SHOW
    else if (legalNum && strncmp_P(smsCmd, GPSDOG_TXT_STORE, 5) && count >= 2) {
        this->readStoreFromSMS();
    }
    // ALARM ON/OFF
    else if (legalNum && strncmp_P(smsCmd, GPSDOG_TXT_ALARM, 5) && count == 1) {
        this->readModeFromSMS(GPSDOG_MODE_ALARM);
    }
    // WATCH ON/OFF
    else if (legalNum && strncmp_P(smsCmd, GPSDOG_TXT_WATCH, 5) && count == 1) {
        this->readModeFromSMS(GPSDOG_MODE_WATCH);
    }
    // PROTECT ON/OFF
    else if (legalNum && strncmp_P(smsCmd, GPSDOG_TXT_PROTECT, 7) && count == 1) {
        this->readModeFromSMS(GPSDOG_MODE_PROTECT);
    }
    else {
        this->createDefaultSMS(GPSDOG_OPT_SMS_UNKNOWN);
    }

    ////
    // Send Answer
    for (uint8_t i = 0; !this->cb_sendSMS && i < GPSDOG_TRY_SENDSMS; i++) {
        delay(GPSDOG_WAIT_SENDSMS);
    }
}

void GPSDog::updateGPSData(double latitude, double longitude, double speed, char *date, char *time)
{
    bool    newAlarm = false;

    ////
    // GPSDog Watch ON / Check of state change
    if (this->isModeOn(GPSDOG_MODE_WATCH) && !this->isModeOn(GPSDOG_MODE_ALARM)) {

        // Position change
        if (m_latitude != latitude || m_longitude != longitude) {
            // set Alarm
            newAlarm = true;
        }
    }

    ////
    // Copy new Data
    m_latitude  = latitude;
    m_longitude = longitude;
    m_speed     = speed;

    this->copyDateTime(date, time);

    ////
    // create new alarm
    if (newAlarm) {
        this->setMode(GPSDOG_MODE_ALARM, true);

        // send Alarm SMS
        this->sendAlarmSMS();
    }
}

void GPSDog::sendAlarmSMS()
{
    // generate Status SMS Text
    this->createStatusSMS();

    // find numbers where have a active alarm
    for (uint8_t i = 0; i < GPSDOG_CONF_NUMBER_STORE; i++) {

        // Alarm Notify is On
        if (this->isAlarmNotifyOn(i)) {
            // Send Status SMS
            if(this->setNumber(m_numbers[i])) {
                this->cb_sendSMS;
            }
        }
    }

    // calc next alarm SMS
    this->calcNextAlarm();
}


void GPSDog::calcNextAlarm()
{
    uint16_t    now = millis();

    // test of overloaded millis
    if (now + GPSDOG_ALARM_INTERVAL < now) {
        // calc overloaded interval
        m_nextAlarmSMS = now - (0xFFFF - GPSDOG_ALARM_INTERVAL);
    }
    else {
        // calc normal interval
        m_nextAlarmSMS = now + GPSDOG_ALARM_INTERVAL;
    }
}

void GPSDog::createStatusSMS()
{
    char lat[12];
    char lon[12];
    char speed[6];
    char stat[7];

    // init buffer sms text
    if (!this->cleanSMS()) {
        return;
    }

    // convert value
    this->getLatitude(lat, 12);
    this->getLongitude(lat, 12);
    this->getSpeed(speed, 12);

    ////
    // Copy status
    memset(stat, 0x00, 7);
    if (this->isModeOn(GPSDOG_MODE_ALARM)) {
        strncpy_P(stat, GPSDOG_TXT_ALARM, 6);
    }
    else if (this->isModeOn(GPSDOG_MODE_WATCH)) {
        strncpy_P(stat, GPSDOG_TXT_WATCH, 6);
    }
    else {
        strncpy_P(stat, GPSDOG_TXT_STATUS, 6);
    }

    ////
    // create SMS Text
    snprintf_P(m_message, m_messageSize, GPSDOG_SMS_STATUS, stat, lat, lon, speed, m_date, m_time, lat, lon);
}

void GPSDog::createDefaultSMS(uint8_t msgOpt)
{
    // init buffer sms text
    if (!this->cleanSMS()) {
        return;
    }

    switch (msgOpt) {
        case GPSDOG_OPT_SMS_DONE :
            strncpy_P(m_message, GPSDOG_SMS_DONE, m_messageSize);
            break;
        case GPSDOG_OPT_SMS_ERROR :
            strncpy_P(m_message, GPSDOG_SMS_SYSERROR, m_messageSize);
            break;
        case GPSDOG_OPT_SMS_UNKNOWN :
            strncpy_P(m_message, GPSDOG_SMS_UNKNOWN, m_messageSize);
            break;
    }
}

bool GPSDog::parseOnOff(uint8_t idx)
{
    char *onOff = this->getParseElementUpper(idx);

    // ON
    if (strncmp_P(onOff, GPSDOG_TXT_ON, 2) == 0) {
        return true;
    }

    return false;
}

void GPSDog::readModeFromSMS(uint8_t mode)
{
    // set mode
    this->setMode(mode, this->parseOnOff(1));

    // if mode unlike ALARM, write status change to eeprom
    if (mode ^ GPSDOG_MODE_ALARM != 0x00) {
        this->writeConfig();
    }

    // create SMS answer
    this->createDefaultSMS(GPSDOG_OPT_SMS_DONE);
}

void GPSDog::readInitFromSMS()
{
    char *pw            = this->getParseElement(1);
    char *number        = this->getParseElement(2);
    bool alarmNotify    = this->parseOnOff(3);

    // allready init
    if (this->isModeOn(GPSDOG_MODE_INIT)) {
        goto Error;
    }

    // set number to first store
    if (!this->addNumberWithNotify(0, number, alarmNotify)) {
        goto Error;
    }

    // write passwort
    if (!this->setPasswordInit(pw)) {
        goto Error;
    }

    // write config
    this->writeConfig();
    return;

Error:
    this->createDefaultSMS(GPSDOG_OPT_SMS_ERROR);
    return;
}

void GPSDog::readResetFromSMS()
{
    char *pw = this->getParseElement(1);

    // init mode is set and passsword is okay
    if (!this->isModeOn(GPSDOG_MODE_INIT) || !this->checkPassword(pw)) {
        this->createDefaultSMS(GPSDOG_OPT_SMS_ERROR);
        return;
    }

    // reset config
    this->cleanConfig();

    // save config
    this->writeConfig();

    // end
    this->createDefaultSMS(GPSDOG_OPT_SMS_DONE);
}

void GPSDog::readStoreFromSMS()
{
    uint8_t idx     = atoi(this->getParseElement(1)) -1;
    char    *cmd    = this->getParseElementUpper(2);

    // Store number in range
    if (idx >= GPSDOG_CONF_NUMBER_STORE) {
        goto Error;
    }

    // STORE num ADD number ON/OFF
    if (strncmp_P(cmd, GPSDOG_TXT_ADD, 3) == 0) {
        char *number   = this->getParseElement(3);
        bool notify    = this->parseOnOff(4);
    
        // Ckeck number is set
        if (number == NULL || !this->addNumberWithNotify(idx, number, notify)) {
            goto Error;
        }
       
        this->writeConfig();
        this->createDefaultSMS(GPSDOG_OPT_SMS_DONE);
        return;
    }
    // STORE num DEL
    else if (strncmp_P(cmd, GPSDOG_TXT_DEL, 3) == 0) {
        // clean number
        memset(m_numbers[idx], 0x00, GPSDOG_CONF_NUM_SIZE +1);
        this->setAlarmNotify(idx, false);

        // end
        this->writeConfig();
        this->createDefaultSMS(GPSDOG_OPT_SMS_DONE);
        return;
    }
    // STORE num SHOW
    else if (strncmp_P(cmd, GPSDOG_TXT_SHOW, 4) == 0) {
        char onOff[4];

        // init buffer
        memset(onOff, 0x00, 4);

        // Generate text for alarm Notify
        if (this->isAlarmNotifyOn(idx)) {
            strncpy_P(onOff, GPSDOG_TXT_ON, 3);
        }
        else {
            strncpy_P(onOff, GPSDOG_TXT_OFF, 3);
        }

        // write
        snprintf_P(m_message, m_messageSize, GPSDOG_SMS_STORESHOW, m_numbers[idx], onOff);
        return;
    }

Error:
    this->createDefaultSMS(GPSDOG_OPT_SMS_ERROR);
    return;
}

// vim: set sts=4 sw=4 ts=4 et:
