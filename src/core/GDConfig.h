
#ifndef GDCONFIG_H
#define GDCONFIG_H

// includes
#include <EEPROM.h>
#include <inttypes.h>
#include <string.h>

// Buffer Size
// String buffer +1
#define GPSDOG_CONF_PW_SIZE 8
#define GPSDOG_CONF_NUM_SIZE 10

// config
#define GPSDOG_CONF_NUMBER_STORE 0x04
#define GPSDOG_CONF_ALARM_INTERVAL  15 // Min

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
    /** Config struct version */
    uint8_t m_version;

    /** Mode/State of GPSDog (Binary) */
    uint8_t m_mode;

    /** Password for branding */
    char    m_password[GPSDOG_CONF_PW_SIZE +1];

    /** Interval in minutes for sending alarm notify */
    uint8_t m_alarmInterval;

    /** Holding notify information for number in store (Binary) */
    uint8_t m_alarmNumbers;

    /** Save sign count per phone number */
    uint8_t m_signNums[GPSDOG_CONF_NUMBER_STORE];

    /** Store for numbers they have full access to GPSDog */
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

        /** Config data */
        GD_DATA m_data;

    public:

        /**
         * Read Config data and reset it is the config version not correct.
         */
        GDConfig();

        /** Array for easy access to number store from data struct */
        char *m_numbers[GPSDOG_CONF_NUMBER_STORE];

        /**
         * Delete all config data and set it to default.
         */
        void cleanConfig();

        /**
         * Read data form EEPROM.
         */
        void readConfig();

        /**
         * Write data to EEPROM
         */
        void writeConfig();

        /**
         * Write a new number to config store.
         * 
         * @param numStoreIdx           Index of config store number.
         * @param num                   Number to store
         * @param sign                  Sign for this number
         * @return                      TRUE / FALSE if number store
         */
        bool setStoreNumber(uint8_t numStoreIdx, char *num, uint8_t sign);

        /**
         * Check is the number equal to the number in config store.
         *
         * @param numStoreIdx           Index of config store number.
         * @param num                   Number for check
         * @return                      TRUE / FALSE is the number equal
         */
        bool checkStoreNumber(uint8_t numStoreIdx, char *num);

        /**
         * Search in store for the number.
         *
         * @param num                   Number for search
         * @return                      TRUE / FALSE if number is found
         */
        bool foundNumberInStore(char *num);

        /**
         * Add a new number to config store with notify information.
         *
         * @param numStoreIdx           Index of config store number.
         * @param num                   Number they will store
         * @param sign                  Sign for this number
         * @param notify                TRUE/FALSE for ON/OFF alarm notify
         * @return                      TRUE if success
         */
        bool addNumberWithNotify(uint8_t numStoreIdx, char *num, uint8_t sign, bool notify);

        /**
         * Get the sign from the number.
         *
         * @param numStoreIdx           Index of config store number.
         * @return                      The sign of this number
         */
        uint8_t getSignNumber(uint8_t numStoreIdx);

        /**
         * Check is alarm notify active on config store number.
         *
         * @param numStoreIdx           Index of config store number.
         * @return                      TRUE/FALSE for ON/OFF alarm notify
         */
        bool isAlarmNotifyOn(uint8_t numStoreIdx);

        /**
         * Set alarm notify on a number in config store.
         *
         * @param numStoreIdx           Index of config store number.
         * @param onOff                 TRUE/FALSE for ON/OFF alarm notify
         */
        void setAlarmNotify(uint8_t numStoreIdx, bool onOff);

        /**
         * Write a password to store an set it as Init (branding).
         *
         * @param pw                    Password for branding
         * @return                      TRUE if success
         */
        bool setPasswordInit(char *pw);

        /**
         * Check is the password equal to the password in storage.
         *
         * @param pw                    Password for check of equal
         * @return                      TRUE is equal or FALSE
         */
        bool checkPassword(char *pw);

        /**
         * Check is Mode active. Possible modes are:
         * - GPSDOG_MODE_INIT
         * - GPSDOG_MODE_WATCH
         * - GPSDOG_MODE_ALARM
         * - GPSDOG_MODE_PROTECT
         *
         * @param mode                  Mode they will check
         * @return                      TRUE is active or FALSE
         */
        bool isModeOn(uint8_t mode);

        /**
         * Set mode on or off. @see isModeOn.
         *
         * @param mode                  Mode they will do
         * @param onOff                 TRUE for on and FALSE for off
         */
        void setMode(uint8_t mode, bool onOff);

        /**
         * Get alarm interval in config store in minutes
         */
        uint8_t getAlarmInterval() {
            return m_data.m_alarmInterval;
        }

        /**
         * Set alarm interval in config store in minutes
         */
        void setAlarmInterval(uint8_t val) {
            m_data.m_alarmInterval = val;
        }
};

#endif

// vim: set sts=4 sw=4 ts=4 et:
