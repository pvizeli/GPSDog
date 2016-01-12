
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

// String
#define GPSDOG_TXT_STATUS PSTR("STATUS")
#define GPSDOG_TXT_STORE PSTR("STORE")
#define GPSDOG_TXT_ALARM PSTR("ALARM")
#define GPSDOG_TXT_PROTECT PSTR("PROTECT")
#define GPSDOG_TXT_WATCH PSTR("WATCH")
#define GPSDOG_TXT_INIT PSTR("INIT")
#define GPSDOG_TXT_RESET PSTR("RESET")
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

// config
#define GPSDOG_TRY_SENDSMS 3
#define GPSDOG_WAIT_SENDSMS 30000 // 30sec
#define GPSDOG_ALARM_INTERVAL 600000 // 10min
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

        /** */
        bool        m_isInit;

        /** */
        uint16_t    m_nextAlarmSMS;

        /**
         * Callback for sending SMS with GPSDog.
         * @return              TRUE / FALSE if message send.
         */
        bool (*cb_sendSMS);

        /**
         * Callback for check new SMS. Load every new message and
         * call @see processIncomingSMS. After that you cann delete
         * the SMS.
         */
        void (*cb_checkNewSMS);

        /**
         * Callback for receive GPS Data. If receive Data call 
         * @see updateGPSData for precessing.
         */
        void (*cb_receiveGPS);

        /**
         *
         */
        void sendAlarmSMS();

        /**
         *
         */
        void calcNextAlarm();

        /**
         *
         */
        void createStatusSMS();

        /**
         *
         */
        void createDefaultSMS(uint8_t msgOpt);

        /**
         *
         */
        bool parseOnOff(uint8_t idx);

        /**
         *
         */
        void readModeFromSMS(uint8_t mode);

        /**
         *
         */
        void readInitFromSMS();

        /**
         *
         */
        void readResetFromSMS();

        /**
         *
         */
        void readStoreFromSMS();

    public:

        GPSDog();
        
        /**
         *
         */
        void initialize(char *smsNum, uint8_t smsNumSize, char *smsTxt, uint8_t smsTxtSize, bool (*cbSendSMS), void (*cbCheckSMS), void (*cbReceiveGPS));

        /**
         *
         */
        void mainProcessing();

        /**
         *
         */
        void processIncomingSMS();

        /**
         *
         */
        void updateGPSData(double latitude, double longitude, double speed, char *date, char *time);

};

#endif

// vim: set sts=4 sw=4 ts=4 et:
