#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include "Arduino.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "SSD1306.h"

SSD1306  ecran(0x3c, SDA_OLED, SCL_OLED, RST_OLED);
TinyGPSPlus gps;

typedef union {
  float f[3];
  unsigned char bytes[12];
} floatArr2Val;

floatArr2Val latlong;
float latitude;
float longitude;
float alt;

floatArr2Val values;

#define GPS_RX 22
#define GPS_TX 23
HardwareSerial GPSSerial(1);


const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = {26, 34, 35},
};

// DEVEUI à changer ici
static const u1_t PROGMEM DEVEUI[8] = { 0x5D, 0x14, 0xE0, 0xBB, 0x36, 0xF9, 0x81, 0x61 };

void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}

 

// APPEUI à changer ici
static const u1_t PROGMEM APPEUI[8] = { 0x81, 0xE2, 0x50, 0x13, 0x2C, 0xF9, 0x81, 0x61 };

void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}

 

// App key à changer ici
static const u1_t PROGMEM APPKEY[16] = { 0x81, 0x74, 0x85, 0xBB, 0xC6, 0xA7, 0x81, 0x58, 0xEC, 0xEC, 0xFE, 0xFB, 0x9A, 0xC6, 0x46, 0xE4 };

void os_getDevKey (u1_t* buf) {  memcpy_P(buf, APPKEY, 16);}

// UPLINK toutes les 15 secondes
const unsigned TX_INTERVAL = 15;


void do_send(osjob_t* j){

    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("OP_TXRXPEND, not sending"));
    } else {
        byte payload[12];

          get_coords();
          payload[0] = latlong.bytes[0];
          payload[1] = latlong.bytes[1];
          payload[2] = latlong.bytes[2];
          payload[3] = latlong.bytes[3];
          payload[4] = latlong.bytes[4];
          payload[5] = latlong.bytes[5];
          payload[6] = latlong.bytes[6];
          payload[7] = latlong.bytes[7];
          payload[8] = latlong.bytes[8];
          payload[9] = latlong.bytes[9];
          payload[10] = latlong.bytes[10];
          payload[11] = latlong.bytes[11];


          for(int i=0;i<12;i++){
            Serial.print(payload[i], HEX);
          }
          Serial.println();
      
        LMIC_setTxData2(1, payload, sizeof(payload), 0);
        Serial.println(F("Payload queued"));

    }

}

static osjob_t sendjob;

void onEvent (ev_t ev) {

    switch(ev) {

        case EV_JOINING:

            Serial.println("EV_JOINING");

            break;

        case EV_JOINED:

            Serial.println("EV_JOINED");

            LMIC_setLinkCheckMode(0);

            break;

        case EV_JOIN_FAILED:

            Serial.println("EV_JOIN_FAILED");

            break;

        case EV_REJOIN_FAILED:

            Serial.println("EV_REJOIN_FAILED");

            break;

        case EV_TXCOMPLETE:

            Serial.println("EV_TXCOMPLETE");

            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);

            break;

         default:

            break;

    }

}

void setup() {

    Serial.begin(9600);
    while(!Serial);
    SPI.begin(SCK,MISO,MOSI,SS);
    Serial.println(F("Starting"));
    GPSSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

    digitalWrite(Vext,LOW);
    delay(20);
    ecran.init();
    delay(20);
    ecran.wakeup();
    ecran.setFont(ArialMT_Plain_16);
    ecran.setTextAlignment(TEXT_ALIGN_CENTER);
    ecran.clear();
    ecran.drawString(64, 15, "IOT LOUVIERS");
    ecran.drawString(64, 30, "MAPPER v0.1");
    ecran.display();

    os_init();

    LMIC_reset();

    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);

    LMIC_setLinkCheckMode(0);

    LMIC_setDrTxpow(DR_SF7, 14);

    do_send(&sendjob);

}

 

void loop() {

    os_runloop_once();

}

void get_coords () {
  while (GPSSerial.available())
    gps.encode(GPSSerial.read());
  latitude  = gps.location.lat();
  longitude = gps.location.lng();
  alt = gps.altitude.meters();
  
  if ((latitude && longitude) && latitude != latlong.f[0]
      && longitude != latlong.f[1]) {
    latlong.f[0] = latitude;
    latlong.f[1] = longitude;
    latlong.f[2] = alt;
    
  }
}
