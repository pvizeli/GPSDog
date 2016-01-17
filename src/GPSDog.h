
#ifndef GPSDOG_H
#define GPSDOG_H

// includes
#include <Arduino.h>
#include <inttypes.h>
#include <string.h>
#include <avr/pgmspace.h>

// includes GPSDog
#include "core/GDConfig.h"
#include "core/GDSms.h"
#include "core/GDGps.h"

// ASCII
#define GPSDOG_CHAR_ASK 0x3f

// String
#define GPSDOG_TXT_STATUS PSTR("STATUS")
#define GPSDOG_TXT_STORE PSTR("STORE")
#define GPSDOG_TXT_ALARM PSTR("ALARM")
#define GPSDOG_TXT_PROTECT PSTR("PROTECT")
#define GPSDOG_TXT_WATCH PSTR("WATCH")
#define GPSDOG_TXT_INIT PSTR("INIT")
#define GPSDOG_TXT_RESET PSTR("RESET")
#define GPSDOG_TXT_INTERVAL PSTR("INTERVAL")
#define GPSDOG_TXT_STOP PSTR("STOP")
#define GPSDOG_TXT_ADD PSTR("ADD")
#define GPSDOG_TXT_DEL PSTR("DEL")
#define GPSDOG_TXT_SHOW PSTR("SHOW")
#define GPSDOG_TXT_ON PSTR("ON")
#define GPSDOG_TXT_OFF PSTR("OFF")

#define GPSDOG_SMS_STORESHOW PSTR("Number: %s\x0D\x0A" \
                                  "Alarm: %S")
#define GPSDOG_SMS_DONE PSTR("Done")
#define GPSDOG_SMS_UNKNOWN PSTR("Command unknown!")
#define GPSDOG_SMS_SYSERROR PSTR("System Error!")
#define GPSDOG_SMS_INIT PSTR("GPSDog is ready to use")
#define GPSDOG_SMS_STATUS PSTR("State: %s\x0D\x0A" \
                               "Lat: %s\x0D\x0A" \
                               "Long: %s\x0D\x0A" \
                               "Speed: %s\x0D\x0A" \
                               "Period: %s %s\x0D\x0A" \
                               "https://www.google.com/maps?q=%s,%s")

// opt
#define GPSDOG_OPT_SMS_DONE 0x01
#define GPSDOG_OPT_SMS_ERROR 0x02
#define GPSDOG_OPT_SMS_UNKNOWN 0x03
#define GPSDOG_OPT_SMS_INIT 0x04

// config
#define GPSDOG_TRY_SENDSMS 3
#define GPSDOG_WAIT_SENDSMS 30000 // 30sec
#define GPSDOG_WAIT_PROCESSING 30000 // 30sec

/**
 * Object for GPSDog config
 */
class GPSDog :
    protected GDConfig,
    protected GDGps,
    protected GDSms
{
    private:

        /** All callback function and buffer are set */
        bool        m_isInit;

        /** Millis value of next alarm processing */
        uint32_t    m_nextAlarmSMS;
        uint32_t    m_alarmStartTime;
        bool        m_alarmOverload;

        /**
         * Callback for sending SMS with GPSDog.
         * @return              TRUE / FALSE if message send.
         */
        bool (*cb_sendSMS)();

        /**
         * Callback for check new SMS. Load every new message and
         * call @see processIncomingSMS. After that you cann delete
         * the SMS.
         */
        void (*cb_checkNewSMS)();

        /**
         * Callback for receive GPS Data. If receive Data call 
         * @see updateGPSData for precessing.
         */
        void (*cb_receiveGPS)();

        /**
         * Send a status alarm SMS to all number in store with active
         * notify state. It also set @see m_nextAlarmSMS.
         */
        void sendAlarmSMS();

        /**
         * Calc the next milli value for resend the alarm to all numbers.
         * @see sendAlarmSMS.
         */
        void calcNextAlarm();

        /**
         * Create SMS text with status.
         */
        void createStatusSMS();

        /**
         * Create a default SMS text with this optons:
         * - GPSDOG_OPT_SMS_DONE
         * - GPSDOG_OPT_SMS_ERROR
         * - GPSDOG_OPT_SMS_UNKNOWN
         * - GPSDOG_OPT_SMS_INIT
         *
         * @param msgOpt            See list above.
         */
        void createDefaultSMS(uint8_t msgOpt);

        /**
         * Write the state (ON/OFF) to a SMS text.
         *
         * @param mode              Mode of written state.
         */
        void createModeStateSMS(uint8_t mode);

        /**
         * Parse ON/OFF from a incoming SMS to a boolean.
         *
         * @param idx               @see getParseElementUpper.
         */
        bool parseOnOff(uint8_t idx);

        /**
         * Write ON or OFF to a char buffer.
         * 
         * @param buffer            Buffer to write the text
         * @param size              Size of buffer
         * @param onOff             TRUE = ON / FALSE = OFF
         */
        void textOnOff(char *buffer, uint8_t size, bool onOff);

        /**
         * Parse incoming SMS for mode set/read functionality.
         *
         * @param mode              Mode of set/read state.
         */
        void readModeFromSMS(uint8_t mode);

        /**
         * Parse incoming SMS for initial functionality.
         */
        void readInitFromSMS();

        /**
         * Parse incoming SMS for reset functionality.
         */
        void readResetFromSMS();

        /**
         * Parse incoming SMS for store functionality.
         */
        void readStoreFromSMS();

        /**
         * Parse incoming SMS for interval functionality.
         */
        void readIntervalFromSMS();

    public:

        GPSDog();
        
        /**
         * Initialize the Dog with all external data they needed to work.
         *
         * @param smsNum                Pointer to SMS buffer for number
         * @param smsNumSize            Size of SMS number buffer
         * @param smsTxt                Pointer to SMS buffer for message
         * @param smsTxtSize            Size of SMS message buffer
         * @param cbSendSMS             Callback function for send SMS
         * @param cbCheckSMS            Callback function for check new SMS
         * @param cbReceiveGPS          Callback function for update GPS pos
         */
        void initialize(char *smsNum, uint8_t smsNumSize, char *smsTxt, uint8_t smsTxtSize, bool (*cbSendSMS)(), void (*cbCheckSMS)(), void (*cbReceiveGPS)());

        /**
         * Main program loop.
         */
        void mainProcessing();

        /**
         * Call this function for a new SMS in SMS buffer avilable for
         * processing.
         */
        void processIncomingSMS();

        /**
         * Call this function for update the device location.
         */
        void updateGPSData(double latitude, double longitude, double speed, char *date, char *time);

};

#endif

// vim: set sts=4 sw=4 ts=4 et:
