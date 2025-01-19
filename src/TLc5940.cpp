#include"Tlc5940.h"

volatile int XLAT_FLAG = 0;
volatile int DEBUG = 0;
hw_timer_t* blankTimer;



static void IRAM_ATTR onBlank() {
    DEBUG++;
    digitalWrite(BLANK_PIN, HIGH);
    if (XLAT_FLAG) {
        XLAT_FLAG = 0;
        digitalWrite(XLAT_PIN, HIGH);
        digitalWrite(XLAT_PIN, LOW);
    }
    digitalWrite(BLANK_PIN, LOW);
}

void Tlc5940::init(uint16_t initialValue)
{
    /* Pin Setup */
    pinMode(BLANK_PIN, OUTPUT);
    pinMode(XLAT_PIN, OUTPUT);   // ESP32-C3-kompatibler CS-Pin

    pinMode(HSPI_MISO, OUTPUT);
    pinMode(HSPI_MOSI, OUTPUT);
    pinMode(HSPI_SCLK, OUTPUT);  
    
    // SPI Setup
    SPI.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, -1); // C3-SPI-Konfiguration
    SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE3));
    
    // Timer Stuf 
    pinMode(GSCLK_PIN, OUTPUT);  
    ledcSetup(GSCLK_TIMER, GSCLK_FREQ, GSCLK_TIMER_RESULUTION);
    ledcAttachPin(GSCLK_PIN, GSCLK_TIMER);
    ledcWrite(GSCLK_TIMER, 1);


    // Hardware-Timer
    blankTimer = timerBegin(0 , 80, true); // Timer 0 auf dem ESP32-C3
    timerAttachInterrupt(blankTimer, &onBlank, true);
    timerAlarmWrite(blankTimer, 4096, true);
    timerAlarmEnable(blankTimer);

    setAll(initialValue);
    update();
}

Tlc5940::Tlc5940(uint8_t amount_tlc) {
    this->amount_tlc = amount_tlc;
}

uint8_t Tlc5940::update(){
    Serial.print("TLC: ");
    Serial.println(DEBUG);
    if (XLAT_FLAG) {
        return 1;
    }

  for (int tlc_num = 0; tlc_num < NUM_TLCS; tlc_num++) {
    for (int tlc_index = 0; tlc_index < 24; tlc_index++) {
      byte b = this->tlc_GSData[tlc_num*24 + tlc_index];
      SPI.transfer(b);
    }
  }

    XLAT_FLAG = 1;

  return 0;
}

void Tlc5940::set(uint8_t channel, uint16_t value) {
    byte index8 = (NUM_TLCS * 16 - 1) - channel;
    uint8_t *index12p = this->tlc_GSData + ((((uint16_t)index8) * 3) >> 1);
    if (index8 & 1) { // starts in the middle
                      // first 4 bits intact | 4 top bits of value
        *index12p = (*index12p & 0xF0) | (value >> 8);
                      // 8 lower bits of value
        *(++index12p) = value & 0xFF;
    } else { // starts clean
                      // 8 upper bits of value
        *(index12p++) = value >> 4;
                      // 4 lower bits of value | last 4 bits intact
        *index12p = ((uint8_t)(value << 4)) | (*index12p & 0xF);
    }
}

void Tlc5940::setAll(uint16_t value) {
{
    byte firstByte = value >> 4;
    byte secondByte = (value << 4) | (value >> 8);
    byte *p = this->tlc_GSData;
    while (p < this->tlc_GSData + NUM_TLCS * 24) {
        *p++ = firstByte;
        *p++ = secondByte;
        *p++ = (byte)value;
    }
}
}

void Tlc5940::disable(){
    timerAlarmDisable(blankTimer);
    // ledcWrite(2, 4095);
}

void Tlc5940::enable(){
    // ledcWrite(2, 2);
    timerAlarmEnable(blankTimer);
}

void Tlc5940::clear(void)
{
    setAll(0);
}

uint8_t* Tlc5940::getGSData() {
    return this->tlc_GSData;
}
