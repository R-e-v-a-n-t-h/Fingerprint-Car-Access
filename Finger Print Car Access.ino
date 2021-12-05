/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>
int engine0=4,door0=6,trunk0=8,engine1=5,door1=7,trunk1=9,doorTimer=0,trunkTimer=0;
int engineRelay=A0, doorRelay=A1,trunkRelay =A2;
bool engineOn=false,doorOpen=false,trunkOpen=false;

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

pinMode(13,OUTPUT);
pinMode(12,OUTPUT);
pinMode(engine0,OUTPUT);
pinMode(engine1,OUTPUT);
pinMode(door0,OUTPUT);
pinMode(door1,OUTPUT);
pinMode(trunk0,OUTPUT);
pinMode(trunk1,OUTPUT);
pinMode(engineRelay,OUTPUT);
pinMode(doorRelay,OUTPUT);
pinMode(trunkRelay,OUTPUT);
}

void loop()                     // run over and over again
{
  digitalWrite(13,HIGH);    //Extra 5V pins 12 and 13
  digitalWrite(12,HIGH);

// Starting and stopping mentioned operation*********************************
//Engine 
  if(engineOn==true){
    digitalWrite(engine1,HIGH);
    digitalWrite(engine0,LOW);
    digitalWrite(engineRelay,LOW);
  }
  else{
    digitalWrite(engine1,LOW);
    digitalWrite(engine0,HIGH);
    digitalWrite(engineRelay,HIGH);
  }

//Door
  if(doorOpen==true){
    digitalWrite(door1,HIGH);
    digitalWrite(door0,LOW);
    digitalWrite(doorRelay,LOW);
    doorTimer+=1;
    if(doorTimer==100){
      doorOpen=false;
      doorTimer=0;
    }
  }
  else{
    digitalWrite(door1,LOW);
    digitalWrite(door0,HIGH);
    digitalWrite(doorRelay,HIGH);
  }


//Trunk
if(trunkOpen==true){
    digitalWrite(trunk1,HIGH);
    digitalWrite(trunk0,LOW);
    digitalWrite(trunkRelay,LOW);
    trunkTimer+=1;
    if(trunkTimer==90){
      trunkOpen=false;
      trunkTimer=0;
    }
  }
  else{
    digitalWrite(trunk1,LOW);
    digitalWrite(trunk0,HIGH);
    digitalWrite(trunkRelay,HIGH);
  }
//*****************************************************************************

  
  int fp=getFingerprintIDez();
  
  //Turn off/on or open/close logic with Fingerprint*************************
  if(fp%10==1){
    if (engineOn)
        engineOn=false;
    else
        engineOn=true;
  }

  else if(fp%10==2){
    if (doorOpen)
        doorOpen=false;
    else{
        doorOpen=true;
        doorTimer=0;
    }
  }

  else if(fp%10==3){
    if (trunkOpen)
        trunkOpen=false;
    else{
        trunkOpen=true;
        trunkTimer=0;
    }
  }
//**************************************************************************
  delay(50);            //don't ned to run this at full speed.
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  
  return finger.fingerID;
}
