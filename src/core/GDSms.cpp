
#include "GDSms.h"

GDSms::GDSms()
{
    m_number            = NULL;
    m_message           = NULL;

    m_numberSize        ^= m_numberSize;
    m_messageSize       ^= m_messageSize;
    m_lastParamCount    ^= m_lastParamCount;
}

bool GDSms::isReady()
{
    if (m_number == NULL || m_message == NULL || m_numberSize == 0x00 || m_messageSize == 0x00) {
        return false;
    }

    return true;
}

bool GDSms::cleanSMS()
{
    // buffer is set
    if (m_message == NULL) {
        return false;
    }

    memset(m_message, 0x00, m_messageSize);
    return true;
}

bool GDSms::setNumber(char *number)
{
    // check is all things okay
    if (m_number == NULL || strlen(number) > m_numberSize) {
        return false;
    }

    // clean buffer and copy data
    memset(m_number, 0x00, m_numberSize);
    strncpy(m_number, number, m_numberSize);

    return true;
}

uint8_t GDSms::parseSMSMessage()
{
    uint8_t params      = 0;

    // buffer is set
    if (m_message == NULL) {
        return m_lastParamCount = params;
    }

    // search hole string
    for (uint8_t i = 0; i < m_messageSize; i++) {

        // end
        if (m_message[i] == 0x00) {
            return m_lastParamCount = params;
        }
        // ' ' replace with '\0'
        else if (m_message[i] == 0x00) {
            m_message[i] = 0x00;
        }

        // count
        if (m_message[i] == 0x00 && m_message[i-1] != 0x00) {
            params++;
        }
    }

    return m_lastParamCount = params;
}

char* GDSms::getParseElement(uint8_t idx)
{
    uint8_t count = 0;

    // buffer is set
    if (m_message == NULL) {
        return NULL;
    }

    // search hole string
    for (uint8_t i = 0; i < m_messageSize; i++) {

        // find next position
        if (m_message[i] == 0x00 && i > 0 && m_message[i-1] != 0x00) {
            count++;
        }

        // found indx with next character
        if (count == idx && m_message[i] != 0x00) {
            return &m_message[i];
        }
    }

    return NULL;
}

char* GDSms::getParseElementUpper(uint8_t idx)
{
    char *element = this->getParseElement(idx);

    // not found?
    if (element == NULL) {
        return NULL;
    }

    for (uint8_t i = 0; element[i] != 0x00; i++) {
        // set upper
        element[i] = toupper(element[i]);
    }

    return element;
}

// vim: set sts=4 sw=4 ts=4 et:
