
#ifndef GDCONFIG_H
#define GDCONFIG_H

// includes
#include <EEPROM.h>

// Buffer Size
// String buffer +1
#define GPSDOG_CONF_PW_SIZE 8
#define GPSDOG_CONF_NUM_SIZE 10

// config
#define GPSDOG_CONF_NUMBER_STORE 0x04

// mode
#define GPSDOG_MODE_INIT 0x01
#define GPSDOG_MODE_WATCH 0x02
#define GPSDOG_MODE_ALARM 0x04
#define GPSDOG_MODE_PROTECT 0x08

// Config Version
#define GPSDOG_CONF_VERSION 0x01

/**
 *
 */
struct GD_DATA 
{
    /** */
    uint8_t m_version;

    /** */
    uint8_t m_mode;

    /** */
    char    m_password[GPSDOG_CONF_PW_SIZE +1];

    /** */
    uint8_t m_alarmNumbers;

    /** */
    char    m_number1[GPSDOG_CONF_NUM_SIZE +1];
    char    m_number2[GPSDOG_CONF_NUM_SIZE +1];
    char    m_number3[GPSDOG_CONF_NUM_SIZE +1];
    char    m_number4[GPSDOG_CONF_NUM_SIZE +1];
};

/**
 * Object for GPSDog config
 */
class GDConfig
{
    private:

        /**
         *
         */
        void cleanConfig();

        /** */
        GD_DATA m_data;

    public:

        GDConfig();

        /** */
        char *m_numbers[GPSDOG_CONF_NUMBER_STORE];

        /**
         *
         */
        void readConfig();

        /**
         *
         */
        void writeConfig();

        /**
         *
         */
        bool setStoreNumber(uint8_t numStoreIdx, char *num);

        /**
         *
         */
        bool checkStoreNumber(uint8_t numStoreIdx, char *num);

        /**
         *
         */
        bool foundNumberInStore(char *num);

        /**
         *
         */
        bool isAlarmNotifyOn(uint8_t numStoreIdx);

        /**
         *
         */
        void setAlarmNotify(uint8_t numStoreIdx, bool onOff);

        /**
         *
         */
        bool setPasswordInit(char *pw);

        /**
         *
         */
        bool checkPassword(char *pw);

        /**
         *
         */
        bool isModeOn(uint8_t mode);

        /**
         *
         */
        void setMode(uint8_t mode, bool onOff);
};

#endif

// vim: set sts=4 sw=4 ts=4 et:
