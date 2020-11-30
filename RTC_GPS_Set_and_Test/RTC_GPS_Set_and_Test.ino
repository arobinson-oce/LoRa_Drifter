// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#undef SECONDS_PER_DAY
#include <NMEAGPS.h>
#include <GPSport.h>
#include <SPI.h>


// what's the name of the hardware serial port?
#define GPSSerial Serial1

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

RTC_PCF8523 rtc;

int h = 0;
int m = 0;
int s = 0;


char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {

#ifndef ESP8266
  while (!Serial); // for Leonardo/Micro/Zero
#endif

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

// 9600 baud is the default rate for the Ultimate GPS
  GPSSerial.begin(9600);

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop () {

     while (gps.available( gpsPort )) {
    fix = gps.read();
     // 
    Serial.println( F("Location: ") );
    if (fix.valid.location) {
     Serial.print( fix.latitude(), 6 );
     Serial.print( ',' );
     Serial.println( fix.longitude(), 6 );
     }
     
    h = (fix.dateTime.hours);
    m = (fix.dateTime.minutes);
    s = (fix.dateTime.seconds);
    
    rtc.adjust(DateTime(2020, 10, 14, h, m, s));


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
    Serial.println(dataString);
    Serial.println();
      
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    
    Serial.println();
    //.delay(1000);
}
}
