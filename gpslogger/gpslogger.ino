#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600;
const int chipSelect = 10;
const int ERROR_NO_GPS_LED = 6;
const int ERROR_SD_FILE_PROBLEM = 7;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{  
  Serial.begin(115200);
  ss.begin(GPSBaud);

  pinMode(10, OUTPUT);
  pinMode(ERROR_NO_GPS_LED, OUTPUT);
  pinMode(ERROR_SD_FILE_PROBLEM, OUTPUT);
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  
  Serial.println(F("DeviceExample.ino"));
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println(F("by Mikal Hart"));
  Serial.println();
}


String dataString = "";
String date = "";
String currentTime = "";
String coords = "";
float lat = 0,lng=0;
void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read())){
      displayInfo();
      writeData();
    }
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    digitalWrite(ERROR_NO_GPS_LED, HIGH);
   // while(true);
  } else {
    digitalWrite(ERROR_NO_GPS_LED, LOW);
  }
  // if the file is available, write to it:
  
}
void writeData(){
  File dataFile = SD.open("datalog.csv", FILE_WRITE);
  Serial.println(SD.exists("datalog.csv"));
  Serial.println(dataFile);
  if (dataFile) {
    dataFile.println(coords + ", " + date+ ", " + currentTime);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
    dataFile.close();
    digitalWrite(ERROR_NO_GPS_LED, LOW);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    digitalWrite(ERROR_SD_FILE_PROBLEM, HIGH);
  }
}
void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    
    lat = gps.location.lat();
    lng = gps.location.lng();
    coords = String(gps.location.lat(),6) + "," + String(gps.location.lng(),6);
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    lat = lng = 0.;
    coords = "";
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    date = String(gps.date.month())+"/"+String(gps.date.day()) + "/"+ String(gps.date.year());
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    date="";
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  { 
    if (gps.time.hour() < 10) Serial.print(F("0"));
     currentTime=String(gps.time.hour())+":";
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    currentTime+=String(gps.time.minute())+":";
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    currentTime+=String(gps.time.second())+".";
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    currentTime+=String(gps.time.centisecond());
    Serial.print(gps.time.centisecond());
  }
  else
  {
    currentTime="";
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
