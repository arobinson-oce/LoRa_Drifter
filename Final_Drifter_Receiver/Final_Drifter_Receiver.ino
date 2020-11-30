//======================================================================
//  Final_Drifter_Receiver.ino
//
//  Adafruit Feather M0 w/ 900mHz LoRa module, compatible w/ omnidirectional or directional antenna
//  Constantly listening for transmissions
//    Prints header during setup, to facilitate saving to csv file through terminal shell
//    Waits until full message is received before printing, to avoid char-by-char rewrite of buffer
//
// `2019-07-01 ARR   added comments
//======================================================================

#include <SPI.h>
#include <RH_RF95.h>

//for feather m0  
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3

#if defined(ESP8266)
  /* for ESP w/featherwing */ 
  #define RFM95_CS  2    // "E"
  #define RFM95_RST 16   // "D"
  #define RFM95_INT 15   // "B"

#elif defined(ESP32)  
  /* ESP32 feather w/wing */
  #define RFM95_RST     27   // "A"
  #define RFM95_CS      33   // "B"
  #define RFM95_INT     12   //  next to A

#elif defined(NRF52)  
  /* nRF52832 feather w/wing */
  #define RFM95_RST     7   // "A"
  #define RFM95_CS      11   // "B"
  #define RFM95_INT     31   // "C"
  
#elif defined(TEENSYDUINO)
  /* Teensy 3.x w/wing */
  #define RFM95_RST     9   // "A"
  #define RFM95_CS      10   // "B"
  #define RFM95_INT     4    // "C"
#endif


// Must match RX's freq
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

int charcount = 0;

void setup() 
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }

  delay(2000);  // delay allows for setup of terminal so header prints to csv 

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    //Serial.println("LoRa radio init failed");
    while (1);
  
  }
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    //Serial.println("setFrequency failed");
    while (1);
  }
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  
  rf95.setTxPower(23, false);

//print csv header
Serial.println("lat,lon,time,comment");

}

int16_t packetnum = 0;  // packet counter, we increment per xmission

  
void loop()
{
 
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  
  
  if (rf95.waitAvailableTimeout(1000))
  { 
    // Listen for transmissions
    if (rf95.recv(buf, &len))
   {
    //increase counter by 1 for every characted rec'd
    charcount ++ ;
    
    //once full message rec'd, print
    if(charcount > 40){
        Serial.print((char*)buf);
        Serial.print(rf95.lastRssi(), DEC);    
        Serial.println();
        charcount = 0;
       }
    
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

