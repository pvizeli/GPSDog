
#include "GDGps.h"

void GDGps::GDGps()
{
    memset(m_date, 0x00, GPSDOG_GPS_DATE_SIZE + 1);
    memset(m_time, 0x00, GPSDOG_GPS_TIME_SIZE + 1);

    m_latPos        ^= m_latPos;
    m_longPos       ^= m_longPos;

    m_latitude      = 0;
    m_longitude     = 0;
    m_altitude      = 0;
    m_speed         = 0;
}

void GDGps::copyDateTime(char *date, char *time)
{
    strncpy(m_date, date, GPSDOG_GPS_DATE_SIZE);
    strncpy(m_time, time, GPSDOG_GPS_TIME_SIZE);
}

uint8_t GDGps::getGoolgeMapLink(char *buffer, uint8_t size)
{
    char        lat[12];
    char        lon[12];
    int16_t     count;

    // init
    memset(lat, 0x00, 12);
    memset(lon, 0x00, 12);

    // convert to string
    this->getLatitude(lat, 12);
    this->getLongitude(lon, 12);

    // generate link
    count = snprintf_P(buffer, size, GSPDOG_GPS_GOOGLE_URL, lat, lon); 

    if (count < 0) {
        return 0;
    }

    return static_cast<uint8_t>(count);
}

uint8_t GDGps::getLatitude(char *buffer, uint8_t size)
{
    size_t count;

    // check buffer size are okay
    if (size < 11) {
        return 0;
    }

    // convert
    count =  strlen(dtostrf(m_latitude, 11, 6, buffer));
    return static_cast<uint8_t>(count);
}

uint8_t GDGps::getLongitude(char *buffer, uint8_t size)
{
    size_t count;

    // check buffer size are okay
    if (size < 11) {
        return 0;
    }

    // convert
    count = strlen(dtostrf(m_longitude, 11, 6, buffer));
    return static_cast<uint8_t>(count);
}

uint8_t GDGps::getSpeed(char *buffer, uint8_t size)
{
    size_t count;

    // check buffer size are okay
    if (size < 6) {
        return 0;
    }

    // convert
    count = strlen(dtostrf(m_longitude, 6, 2, buffer));
    return static_cast<uint8_t>(count);
}

// vim: set sts=4 sw=4 ts=4 et:
