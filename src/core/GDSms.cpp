
#include "GDSms.h"

GDSms::GDSms()
{
    m_number        = NULL;
    m_message       = NULL;

    m_numberSize    ^= m_numberSize;
    m_messageSize   ^= m_messageSize;
}

bool GDSms::isReady()
{
    if (m_numer == NULL || m_message == NULL || m_numberSize == 0x00 || m_messageSize == 0x00) {
        return false;
    }

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

// vim: set sts=4 sw=4 ts=4 et:
