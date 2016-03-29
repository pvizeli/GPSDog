#include <GPSDog.h>

GPSDog gpsDog;

/**
 * AT-Device Option  
 */

// SIM5218
#define SIM5218_USE_EASYSMS
#define SIM5218_USE_GPS

#include <ATDev_HW.h>

SIM5218 modem;

/**
 * Other stuff
 */

uint16_t storeIdx = ATDEV_SMS_NO_MSG;


/**
 * Arduino setup scatch
 */
void setup() {
  // init Modem
  modem.initialize(&Serial, 115200, 2);
  modem.onPower();

  // init GPS / Activate GPS
  modem.initializeGPS();
  modem.onGPS();

  // init SMS
  modem.initializeSMS();

  // init GPSDog
  gpsDog.initialize(&(modem.m_smsData.m_number[0]), 
                    static_cast<uint8_t>(ATDEV_SMS_NUM_SIZE), 
                    &(modem.m_smsData.m_message[0]), 
                    static_cast<uint8_t>(ATDEV_SMS_TXT_SIZE),
                    &sendSMS, &checkSMS, &reloadSMS, &receiveGPS);
}

/**
 * Arduino loop scatch
 */
void loop() {
  // processing GPSDog system
  gpsDog.mainProcessing();
}

void sendSMS()
{
  uint8_t state;
  
  // modem response
  if (modem.isReady() != ATDEV_OK) {
    return;
  }
  // Check Network Status
  state = modem.getNetworkStatus();

  // Check if Network if avilable
  if (state != ATDEV_NETSTAT_REGISTERED && state != ATDEV_NETSTAT_ROAMING) {
    return;    
  }

  // Send SMS
  modem.sendSMS();
}

void receiveGPS()
{
  char date[11];
  char time[6];
  
  // modem response
  if (modem.isReady() != ATDEV_OK) {
    return;
  }
  
  if (modem.receiveGPS() == ATDEV_OK) {

    // convert date
    date[0] = modem.m_gpsData.m_date[0];
    date[1] = modem.m_gpsData.m_date[1];
    date[2] = modem.m_gpsData.m_date[2];
    date[3] = modem.m_gpsData.m_date[3];
    date[4] = 0x2D; // -
    date[5] = modem.m_gpsData.m_date[4];
    date[6] = modem.m_gpsData.m_date[5];
    date[7] = 0x2D; // -
    date[8] = modem.m_gpsData.m_date[6];
    date[9] = modem.m_gpsData.m_date[7];
    date[10] = 0x00;

    // convert time
    time[0] = modem.m_gpsData.m_time[0];
    time[1] = modem.m_gpsData.m_time[1];
    time[2] = 0x3A; // :
    time[3] = modem.m_gpsData.m_time[2];
    time[4] = modem.m_gpsData.m_time[3];
    time[5] = 0x00;
    
    // update GPS Data on GPSDog
    gpsDog.updateGPSData(modem.m_gpsData.m_latitude,
                         modem.m_gpsData.m_longitude,
                         modem.m_gpsData.m_speed,
                         date, time);
  }
}

void checkSMS()
{
  bool     hasRead     = false;
  
  // modem response
  if (modem.isReady() != ATDEV_OK) {
    return;
  }

  // read all SMS
  do {
    // read sms index number from store
    storeIdx = modem.readNextIdxSMS();

    // exists a sms
    if (storeIdx != ATDEV_SMS_NO_MSG) {
        hasRead = true;

      // read sms to memorie
      if (modem.receiveSMS(storeIdx) == ATDEV_OK) {

        // proccessing sms on GPSDog
        gpsDog.processIncomingSMS();      
      }

      // delete sms
      if (modem.deleteSMS(storeIdx) != ATDEV_OK) {
        // break
        storeIdx = ATDEV_SMS_NO_MSG;
      }
    }
    
  } while (storeIdx != ATDEV_SMS_NO_MSG);

  // cleanup sms storage
  if (hasRead) {
    modem.deleteAllSMS(ATDEV_SMS_DEL_ALL);
  }

  // reset data
  storeIdx = ATDEV_SMS_NO_MSG;
}

void reloadSMS()
{
  // modem response
  if (modem.isReady() != ATDEV_OK || storeIdx == ATDEV_SMS_NO_MSG) {
    return;
  }

  modem.receiveSMS(storeIdx);
}

