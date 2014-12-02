#include <Adafruit_CC3000.h>
#include "log.h"

// These are the interrupt and control pins
#define ADAFRUIT_CC3000_IRQ   8  // MUST be an interrupt pin!
// These can be any two pins
#define ADAFRUIT_CC3000_VBAT  9
#define ADAFRUIT_CC3000_CS    10  
// Use hardware SPI for the remaining pins
// On an UNO, SCK = 13, MISO = 12, and MOSI = 11
Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT,
                                         SPI_CLOCK_DIV4); // you can change this clock speed but DI

#define WLAN_SSID       "Scalar24"        // cannot be longer than 32 characters!
#define WLAN_PASS       "Fb274Gh@12G1"
// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

/**************************************************************************/
/*!
    @brief  Tries to read the IP address and other connection details
*/
/**************************************************************************/
bool displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    // Useless for this to be in the output buffer, you couldn't see it
    debug_log("Unable to retrieve the IP Address!");
    return false;
  }
  else
  {
    char buff[64];
    sprintf(buff, "IP Addr: %d.%d.%d.%d", (uint) (ipAddress & 0xFF000000) >> 24, (uint) (ipAddress & 0xFF0000) >> 16, (uint) (ipAddress & 0xFF00) >> 8, (uint) (ipAddress & 0xFF));
    debug_log(buff);
    return true;
  }
}

void setup_cc3000() {
  /* Initialise the module */

  cc3000.setPrinter(NULL); //dynamic_cast<Print *>(&LogOut));
  if (!cc3000.begin())
  {
    debug_log("Unable to initialise the CC3000! Check your wiring?");
    while(1);
  }

  /* Attempt to connect to an access point */  
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    debug_log("Failed to connect to AP!");
    while(1);
  }
   
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!, could use the motion counter if i init that first..
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }
}

uint8_t mdns = 0;
void loop_wifi() {
  
  /*
  if (!(motion_countup & 0xF) && !mdns) {
    LogOut.println(F("mDNS send"));
    mdnsAdvertiser(1, "Kitchen", sizeof("Kitchen"));
    mdns = 1;
  } else {
    mdns = 0;
  }*/
}
