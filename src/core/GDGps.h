
#ifndef GDGPS_H
#define GDGPS_H

// includes
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

// data size
// the real size is SIZE+1 for char buffer
#define GPSDOG_GPS_DATE_SIZE 10
#define GPSDOG_GPS_TIME_SIZE 5

/**
 * Object for store gps data
 */
class GDGps
{
    public:

        GDGps();

        /** Latitude */
        double m_latitude;

        /** Longitude */
        double m_longitude;

        /** Speed */
        double m_speed;

        /** Date in format YYYY-MM.DD */
        char m_date[GPSDOG_GPS_DATE_SIZE +1];

        /** Time in format HH:MM */
        char m_time[GPSDOG_GPS_TIME_SIZE +1];
        
        /**
         * Copy Date and Time string to own buffer.
         *
         * @param date              Date String
         * @param time              Time String
         */
        void copyDateTime(char *date, char *time);

        /**
         * Convert Latitude to a string.
         * Buffer Size need to 11 char without '0'.
         *
         * @param buffer            Buffer to Copy latitude
         * @param size              Max Size of buffer
         * @return                  Char they have written to buffer
         */
        uint8_t getLatitude(char *buffer, uint8_t size);

        /**
         * Convert longitude to a string.
         * Buffer Size need to 11 char without '0'.
         *
         * @param buffer            Buffer to Copy longitude
         * @param size              Max Size of buffer
         * @return                  Char they have written to buffer
         */
        uint8_t getLongitude(char *buffer, uint8_t size);

        /**
         * Convert speed to a string.
         * Buffer Size need to 6 char without '0'.
         *
         * @param buffer            Buffer to Copy speed
         * @param size              Max Size of buffer
         * @return                  Char they have written to buffer
         */
        uint8_t getSpeed(char *buffer, uint8_t size);
};


#endif

// vim: set sts=4 sw=4 ts=4 et:
