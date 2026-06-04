#include "main.h"
#include "programm.h"

TableBuff unTable;

/**
 * AT24C32 has 32-byte pages.
 * Page addresses: 0-31, 32-63, ...
 * 0xF00 - 0xFFF reserved (255 bytes).
 */
uint16_t eepromMemoryAddressForHour(uint8_t prg, uint8_t hour){
    // Calculation: 16 bytes per hour record for alignment.
    uint16_t addressPage = (hour) * 16 + (prg - 1) * (16 * 24);  
	return addressPage;
}

/**
 * @brief Write buffer to EEPROM with page boundary safety.
 */
byte eepromWrBuff(uint16_t memoryAddress, const uint8_t* buffer, uint8_t length) {
    uint8_t bytesWritten = 0;
    while (bytesWritten < length) {
        uint8_t bytesToPageBoundary = 32 - (memoryAddress % 32);
        uint8_t chunkLength = min((uint8_t)(length - bytesWritten), bytesToPageBoundary);

        Wire.beginTransmission(EEPROM_I2C_ADDRESS);
        Wire.write((uint8_t)(memoryAddress >> 8));
        Wire.write((uint8_t)(memoryAddress & 0xFF));

        for (uint8_t i = 0; i < chunkLength; ++i) {
            Wire.write(buffer[bytesWritten + i]);
        }

        byte status = Wire.endTransmission();
        if (status != 0) {
            MYDEBUG_PRINT("I2C Write Error at addr "); MYDEBUG_PRINT(memoryAddress);
            MYDEBUG_PRINT(". Status: "); MYDEBUG_PRINTLN(status);
            return status;
        }
        
        delay(EEPROM_WRITE_DELAY);
        
        memoryAddress += chunkLength;
        bytesWritten += chunkLength;
    }
    return 0; 
}

/**
 * @brief Read byte array (buffer) from EEPROM.
 */
void eepromRdBuff(uint16_t memoryAddress, uint8_t* buffer, uint8_t length) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((uint8_t)(memoryAddress >> 8));
  Wire.write((uint8_t)(memoryAddress & 0xFF));
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, (int)length);
  for (uint16_t i = 0; i < length; i++) {
    if (Wire.available()) {
      buffer[i] = Wire.read();
    } else {
      buffer[i] = 0;
    }
  }
}

// ----- Function for default table preparation ----------
void prepareTable(uint8_t prg, int16_t t0on, int16_t t0off, int16_t t1on, int16_t t1off){
    uint8_t flp = 2;
    uint8_t tmr = 0;
    tmr |= flp << 6;
    
    for (size_t i = 0; i < 24; i++){
      if(i <= 4 || i >= 22){  // Night mode
        unTable.spHour.spT0on = t0on - 50;  // -5.0°C
        unTable.spHour.spT0off = t0off - 50;
        unTable.spHour.spT1on = t1on - 50;
        unTable.spHour.spT1off = t1off - 50;
        unTable.spHour.water0run = 0;
        unTable.spHour.water1run = 0;
        unTable.spHour.water2run = 0;
        unTable.spHour.timerFlap = 0;
      }
      else if((i > 4 && i < 8) || (i > 20 && i < 22)){ // Morning/Evening mode
        unTable.spHour.spT0on = t0on - 20;  // -2.0°C
        unTable.spHour.spT0off = t0off - 20;
        unTable.spHour.spT1on = t1on - 20;
        unTable.spHour.spT1off = t1off - 20;
        flp = 1;
        tmr = 60;
        tmr |= flp << 6;
        unTable.spHour.timerFlap = tmr;
      } else { // Day mode
        unTable.spHour.spT0on = t0on;
        unTable.spHour.spT0off = t0off;
        unTable.spHour.spT1on = t1on;
        unTable.spHour.spT1off = t1off;
        unTable.spHour.water0run = 10;
        unTable.spHour.water1run = 20;
        unTable.spHour.water2run = 30;
        flp = 2;
        tmr = 0;
        tmr |= flp << 6;
        unTable.spHour.timerFlap = tmr;
      }
      uint16_t memoryAddress = eepromMemoryAddressForHour(prg, i);
      byte res = eepromWrBuff(memoryAddress, unTable.buffer, sizeof(unTable));
      MYDEBUG_PRINT("HOUR:"); MYDEBUG_PRINT(i); 
      MYDEBUG_PRINT("; ADD:"); MYDEBUG_PRINT(memoryAddress);
      MYDEBUG_PRINT("; RES:"); MYDEBUG_PRINTLN(res);
    }
}

void prepareProg1(){
    MYDEBUG_PRINTLN("PROGRAMM: 1");
    prepareTable(1, 220, 240, 180, 200); // 22.0, 24.0, 18.0, 20.0
}

void prepareProg2(){
    MYDEBUG_PRINTLN("PROGRAMM: 2");
}

void prepareProg3(){
    MYDEBUG_PRINTLN("PROGRAMM: 3");
}

void prepareProg4(){
    MYDEBUG_PRINTLN("PROGRAMM: 4");
}

/**
 * @brief Test EEPROM content and initialize programs if empty.
 */
void testProgs(){
  MYDEBUG_PRINTLN("AT24C32 EEPROM Test.");
  uint16_t memoryAddress = eepromMemoryAddressForHour(1, 0);
  eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
  if(unTable.spHour.spT0on == -1){
    prepareProg1();
    MYDEBUG_PRINTLN("REWRITTEN PROG N1");
  } else MYDEBUG_PRINTLN("PROGRAMM N1 Ok");
}
