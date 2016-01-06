
#ifndef GDSMS_H
#define GDSMS_H

/**
 * Object for store sms data
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

        /** */
        uint8_t m_messageSize;

        /**
         * Callback for sending SMS with GPSDog
         */
        bool (*sendSMS);

        /**
         * Callback for cleaning all SMS Data buffers
         */
        void (*cleanUp);

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
};

#endif

// vim: set sts=4 sw=4 ts=4 et:
