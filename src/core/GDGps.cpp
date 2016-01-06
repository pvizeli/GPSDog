
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

bool GDGps::createGoolgeMapLink(char *buffer, uint8_t size)
{
    char lat[10];
    char lon[10];

    dtostrf();
}

// vim: set sts=4 sw=4 ts=4 et:
