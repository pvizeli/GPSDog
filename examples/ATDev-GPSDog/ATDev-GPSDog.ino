#include <GPSDog.h>

GPSDog gpsDog;

/**
 * AT-Device Option  
 */
#define ATDEV_SMS_TXT_SIZE 160

// SIM5218
#define SIM5218_USE_EASYSMS
#define SIM5218_USE_GPS

#include <ATDev_HW.h>

SIM5218 modem;

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
                    &sendSMS, &checkSMS, &receiveGPS);
}

void loop() {
  // processing GPSDog system
  gpsDog.mainProcessing();
}

bool sendSMS()
{
  uint8_t state;
  
  // modem response
  if (modem.isReady() != ATDEV_OK) {
    return false;
  }
  // Check Network Status
  state = modem.getNetworkStatus();

  // Check if Network if avilable
  if (state != ATDEV_NETSTAT_REGISTERED && state != ATDEV_NETSTAT_ROAMING) {
    return false;    
  }

  // Send SMS
  if (modem.sendSMS() != ATDEV_OK) {
    return false;
  }
}

void receiveGPS()
{
  // modem response
  if (modem.isReady() != ATDEV_OK) {
    return;
  }
  
  if (modem.receiveGPS() == ATDEV_OK) {

    // update GPS Data on GPSDog
    gpsDog.updateGPSData(modem.m_gpsData.m_latitude,
                         modem.m_gpsData.m_longitude,
                         modem.m_gpsData.m_speed,
                         modem.m_gpsData.m_date,
                         modem.m_gpsData.m_time);
  }
}

void checkSMS()
{
  uint8_t storeIdx;
  
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
  modem.deleteAllSMS(ATDEV_SMS_DEL_ALL);
}


