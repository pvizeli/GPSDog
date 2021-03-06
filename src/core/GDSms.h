
#ifndef GDSMS_H
#define GDSMS_H

// includes
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

/**
 * Object for process sms data
 */
class GDSms
{
    public:

        GDSms();

        /** Phone number string */
        char    *m_number;

        /** Phone number buffer size */
        uint8_t m_numberSize;

        /** SMS body string */
        char    *m_message;

        /** SMS body buffer size */
        uint8_t m_messageSize;

        /** Last count number from @see parseSMSMessage */
        uint8_t m_lastParamCount;

        /**
         * Cleaning SMS Text Data buffer.
         *
         * @return              FALSE if no buffer avilable!
         */
        bool cleanSMS();

        /**
         * Set a phone number to buffer.
         *
         * @param number        Copy this number to buffer
         * @return              If function are success
         */
        bool setNumber(char *number);

        /**
         * Check is all data are set for full functionality.
         *
         * @return              TRUE is all data set
         */
        bool isReady();

        /**
         * Parse message for internal processing.
         * It work like strtok with ' '. Set also the first element to
         * uppercase. This is the command to GPSDog.
         *
         * @return              Count of parsed elements
         */
        uint8_t parseSMSMessage();

        /**
         * Get a element of the parsed message.
         * @see parseSMSMessage();
         *
         * @param idx           The element they will have
         * @return              A pointer to this element in message
         */
        char* getParseElement(uint8_t idx);

        /**
         * Get a element of the parsed message as Uppercase.
         * @see parseSMSMessage();
         *
         * @param idx           The element they will have
         * @return              A pointer to element or NULL
         */
        char* getParseElementUpper(uint8_t idx);

        /**
         * Extract GPSDog command from message.
         * Use first @see parseSMSMessage!
         *
         * @return              The parsed GPSDog SMS command
         */
        char* getSMSCommand() {
            return getParseElementUpper(0);
        }
};

#endif

// vim: set sts=4 sw=4 ts=4 et:
