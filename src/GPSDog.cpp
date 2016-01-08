
// includes
#include "GPSDog.h"


void GPSDog::GPSDog()
{
    m_isInit        = false;
    m_nextAlarmSMS  ^= m_nextAlarmSMS;
}
        
void GPSDog::initialize(char *smsNum, uint8_t smsNumSize, char *smsTxt, uint8_t smsTxtSize, bool (*cbSendSMS), void (*cbCheckSMS), void (*cbReceiveGPS))
{
    // init data
    m_number        = smsNum;
    m_numberSize    = smsNumSize;
    m_message       = smsTxt;
    m_messageSize   = smsTxtSize;

    // callbacks
    cb_sendSMS          = cbSendSMS;
    cb_checkNewSMS      = cbCheckSMS;
    cb_checkReceiveGPS  = cbReceiveGPS;

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
        this->cb_checkNewSMS();

        // processing GPS data
        this->cb_receiveGPS();

        // wait for next process
        delay(GPSDOG_WAIT_PROCESSING);
    }
}

void GPSDog::processIncomingSMS()
{

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
            this->setNumber(m_numbers[i]);
            this->cb_sendSMS();
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

    // init buffers
    this->cleanSMS();

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

// vim: set sts=4 sw=4 ts=4 et:
