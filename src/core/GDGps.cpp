
#include "GDGps.h"

GDGps::GDGps()
{
    memset(m_date, 0x00, GPSDOG_GPS_DATE_SIZE + 1);
    memset(m_time, 0x00, GPSDOG_GPS_TIME_SIZE + 1);

    m_latitude      = 0;
    m_longitude     = 0;
    m_speed         = 0;
}

void GDGps::copyDateTime(char *date, char *time)
{
    strncpy(m_date, date, GPSDOG_GPS_DATE_SIZE);
    strncpy(m_time, time, GPSDOG_GPS_TIME_SIZE);
}

uint8_t GDGps::removeSpace(char *buffer, uint8_t size)
{
    bool dataStart = false;

    // no space
    if (buffer[0] == 0x20) {
        // copy data
        for (uint8_t i = 0, y = 0; size > y; i++) {

            // eof
            if (i >= size) {
                buffer[y++] = 0x00;
            }
            // copy data
            else if (dataStart) {
                buffer[y++] = buffer[i];
            }
            else if (buffer[i] != 0x20) {
                dataStart   = true;
                buffer[y++] = buffer[i];
            }
        }
    }

    return static_cast<uint8_t>(strlen(buffer));
}

uint8_t GDGps::getLatitude(char *buffer, uint8_t size)
{
    // check buffer size are okay
    if (size < 12) {
        return 0;
    }

    // convert
    memset(buffer, 0x00, size);
    dtostrf(m_latitude, 11, 6, buffer);

    return this->removeSpace(buffer, size);
}

uint8_t GDGps::getLongitude(char *buffer, uint8_t size)
{
    // check buffer size are okay
    if (size < 12) {
        return 0;
    }

    // convert
    memset(buffer, 0x00, size);
    dtostrf(m_longitude, 11, 6, buffer);

    return this->removeSpace(buffer, size);
}

uint8_t GDGps::getSpeed(char *buffer, uint8_t size)
{
    // check buffer size are okay
    if (size < 7) {
        return 0;
    }

    // convert
    memset(buffer, 0x00, size);
    dtostrf(m_speed, 6, 2, buffer);

    return this->removeSpace(buffer, size);
}

bool GDGps::cmpGeoData(double a, double b, double geoFix)
{
    double val = a - b;

    // negative
    if (val < 0.0) {
        val *= -1;
    }

    // compare with 0.0001 percent
    if (val < geoFix || val == 0.0) {
        return true;
    }

    return false;
}

// vim: set sts=4 sw=4 ts=4 et:
