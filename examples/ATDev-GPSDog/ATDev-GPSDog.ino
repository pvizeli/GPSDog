
#include <GPSDog.h>

GPSDog gpsDog;

////
// AT-Device Option
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

  // init GPS
  modem.initializeGPS();
  modem.onGPS();

  // init SMS
  modem.initializeSMS();

  // init GPSDog
  gpsDog.initialize(&(modem.m_smsData.m_number[0]), static_cast<uint8_t>(ATDEV_SMS_NUM_SIZE), 
                    &(modem.m_smsData.m_message[0]), static_cast<uint8_t>(ATDEV_SMS_TXT_SIZE),
                    &sendSMS, &checkSMS, &receiveGPS);
}

void loop() {
  // processing system
  gpsDog.mainProcessing();
}

bool sendSMS()
{
  if (modem.sendSMS() == ATDEV_OK) {
    return true;
  }

  return false;
}

void receiveGPS()
{
  if (modem.receiveGPS() == ATDEV_OK) {
    // process
  }
}

void checkSMS()
{
  gpsDog.processIncomingSMS();
}


