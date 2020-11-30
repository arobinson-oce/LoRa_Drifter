//======================================================================
//  Final_Drifter_Transmitter.ino
//
//  Adafruit Feather M0 w/ 900mHz LoRa module, Featherwing Data Logging Shield, Ready to Sky M8N GPS.
//  Fixed lat/lon logging issue by defining fix.latitude as string var when adding to datastring
//  Logging on SD card at 5s interval with LogDelay=5000; log directly to csv, header is printed in code setup
//    Records latitude, longitude, date, time, # of satellites, voltage
//  Radio transmission every 120s, using counter to transmit after 24 logs to SD
//    Transmits latitude, longitude, time, name, voltage
//    Configured for receiver to read into csv file for eventual plotting on Google Earth
//
// `2019-07-01 ARR   added comments
//======================================================================

#include <NMEAGPS.h>
#include <GPSport.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <SD.h>

//LoRa Setup--------------------------------------------
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

//GPS Setup -------------------------------------------
// what's the name of the hardware serial port?
#define GPSSerial Serial1

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

//Variables and definitions----------------------------
#define LED 13
#define VBATPIN A7

String X;
int y;
int i;
uint8_t data[42] = "";    // length of data string to be transmitted

//SD card chip select
const int chipSelect = 10;

int LogDelay = 5000;
int TransDelay = 0;
unsigned long TestTime = 0;
unsigned long CurrTime = 0;

void setup() {
  
  //wake up LoRa
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  
  // wait for hardware serial to appear
  while (!Serial);

  // make this baud rate fast enough to we aren't waiting on it
  Serial.begin(115200);

  // 9600 baud is the default rate for the Ultimate GPS
  GPSSerial.begin(9600);

// LoRa initialization
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);


int16_t packetnum = 0;  // packet counter, we increment per xmission

// Set up SD card
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }

  File dataFile = SD.open("GPSlog.csv", FILE_WRITE);

  // print header in the data file
  if (dataFile) {
    dataFile.println("latitude,longitude,date,time,sat,volts");
    dataFile.close();
  }
  
  CurrTime = millis();
  TestTime = millis() + LogDelay;
  
}

void loop() {
  CurrTime = millis();

  //read battery voltage for power consumption tracking
  float measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  
  while (gps.available( gpsPort )) {
    fix = gps.read();
 
  if (CurrTime > TestTime){
    
    // make a string for assembling the data to log, then print to monitor
  String dataString = "";

      dataString += String(fix.latitude(), 8);
      dataString += ",";
      dataString += String(fix.longitude(), 8);
      dataString += ",";

      dataString += (gps.fix().dateTime.month);
      dataString += "/";
      dataString += (gps.fix().dateTime.date);
      dataString += "/";
      dataString += "20";
      dataString +=  (gps.fix().dateTime.year);
      dataString += ",";
        
      if (fix.dateTime.hours < 10)
        dataString += ( '0' );
      dataString += (fix.dateTime.hours);
      dataString += ( ':' );
      if (fix.dateTime.minutes < 10)
        dataString += ( '0' );
      dataString += (fix.dateTime.minutes);
      dataString += ( ':' );
      if (fix.dateTime.seconds < 10)
        dataString += ( '0' );
      dataString += (fix.dateTime.seconds);
      dataString += (",");
      dataString +=(gps.fix().satellites);
      dataString += (",");
      dataString += (measuredvbat);

  // logging the data string and printing to serial monitor
  
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("GPSlog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.csv");
  }
  
  // increase transmission counter by 1, reset next logging time
    TransDelay ++ ;
    TestTime = CurrTime + LogDelay;

    if (TransDelay > 23){ //= 23 for 2 min @ 5s log, 179 for 15 min @ 5s log, 59 for 15 min @ 15s log
      
   //build string to send via LoRa, then print to serial monitor
     X = String(fix.latitude(),6);    // specify n decimal places of coordinate to send
     X += ",";
     X += String(fix.longitude(), 6);
     X += ",";
     if (fix.dateTime.hours < 10)
     X += ( '0' );
     X += (fix.dateTime.hours);
     X += ( ':' );
     if (fix.dateTime.minutes < 10)
     X += ( '0' );
     X += (fix.dateTime.minutes);
     X += ( ':' );
     if (fix.dateTime.seconds < 10)
     X += ( '0' );
     X += (fix.dateTime.seconds);
     X += (",");
     X += "MORIARTY";               // drifter name, change for each unit; use null characters (-,X,0,etc) to keep uniform length
     X += (measuredvbat);
     y = X.length();
     Serial.println(X);
      
      // Send current position, flash red LED when transmitting
      // divide data string into characters, transmit one at a time
      for (i=0; i<y; i++) {
        data[i] = X[i];
        digitalWrite(LED, HIGH);
        rf95.send(data, sizeof(data));
        rf95.waitPacketSent();
        Serial.println("Sent a reply");
        digitalWrite(LED, LOW);
       }
       
      // reset transmission counter
       TransDelay = 0;
    }
  }
 }
}
