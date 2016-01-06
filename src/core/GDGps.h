
#ifndef GDGPS_H
#define GDGPS_H

#define GSPDOG_GPS_GOOGLE_URL PSTR("https://www.google.com/maps?q=%s,%s")

// data size
// the real size is SIZE+1 for char buffer
#define GPSDOG_GPS_DATE_SIZE 6
#define GPSDOG_GPS_TIME_SIZE 8

/**
 * Object for store gps data
 */
class GDGps
{
    public:
        GDGps()

        /** Latitude */
        double m_latitude;

        /** Longitude */
        double m_longitude;

        /** Altidue */
        double m_altitude;

        /** Speed */
        double m_speed;

        /** Date in format YYYYMMDD */
        char m_date[GPSDOG_GPS_DATE_SIZE +1];

        /** Time in format HHMMSS.S */
        char m_time[GPSDOG_GPS_TIME_SIZE +1];
        
        /** N/S */
        char m_latPos;

        /** W/E */
        char m_longPos;

        /**
         * Copy Date and Time string to own buffer.
         *
         * @param date              Date String
         * @param time              Time String
         */
        void copyDateTime(char *date, char *time);

        /**
         *
         *
         * S (lat) / W (long) * -1
         */
        bool createGoolgeMapLink(char *buffer, uint8_t size);
};


#endif

// vim: set sts=4 sw=4 ts=4 et:
