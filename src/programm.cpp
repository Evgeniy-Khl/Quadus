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
void prepareTable(int16_t t0on, int16_t t0off, int16_t t1on, int16_t t1off){
    for (size_t i = 0; i < 24; i++){
      if(i <= 4 || i >= 22){  // Night mode
        unTable.spProg.spT0on = t0on - 50;  // -5.0°C
        unTable.spProg.spT0off = t0off - 50;
        unTable.spProg.spT1on = t1on - 50;
        unTable.spProg.spT1off = t1off - 50;
      }
      else if((i > 4 && i < 8) || (i > 20 && i < 22)){ // Morning/Evening mode
        unTable.spProg.spT0on = t0on - 20;  // -2.0°C
        unTable.spProg.spT0off = t0off - 20;
        unTable.spProg.spT1on = t1on - 20;
        unTable.spProg.spT1off = t1off - 20;
      } else { // Day mode
        unTable.spProg.spT0on = t0on;
        unTable.spProg.spT0off = t0off;
        unTable.spProg.spT1on = t1on;
        unTable.spProg.spT1off = t1off;
      }
      unTable.spProg.water2run = 0;
      unTable.spProg.flapMin = settings.minFlap;
      unTable.spProg.flapMax = settings.maxFlap;
      unTable.spProg.flapCurr = settings.minFlap;

      uint16_t memoryAddress = eepromMemoryAddressForHour(1, i);
      byte res = eepromWrBuff(memoryAddress, unTable.buffer, sizeof(unTable));
      MYDEBUG_PRINT("HOUR:"); MYDEBUG_PRINT(i); 
      MYDEBUG_PRINT("; ADD:"); MYDEBUG_PRINT(memoryAddress);
      MYDEBUG_PRINT("; RES:"); MYDEBUG_PRINTLN(res);
    }
}

void prepareProg(){
    MYDEBUG_PRINTLN("PROGRAMM: 1");
    prepareTable(220, 240, 180, 200); // 22.0, 24.0, 18.0, 20.0
}

/**
 * @brief Test EEPROM content and initialize programs if empty.
 */
void testProgs(){
  MYDEBUG_PRINTLN("AT24C32 EEPROM Test.");
  uint16_t memoryAddress = eepromMemoryAddressForHour(1, 0);
  eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
  if(unTable.spProg.spT0on == -1){
    prepareProg();
    MYDEBUG_PRINTLN("REWRITTEN PROG N1");
  } else MYDEBUG_PRINTLN("PROGRAMM N1 Ok");
}

void checkAndApplyHourlyProgram() {
    if (settings.program == 0) return;
    if (!RTCENABLE || !timeinfo) return;

    static int8_t lastHour = -1;
    static uint8_t lastProgram = 0;
    uint8_t currentHour = timeinfo->tm_hour;
    uint8_t currentProgram = settings.program;

    if (currentHour != lastHour || currentProgram != lastProgram) {
        lastHour = currentHour;
        lastProgram = currentProgram;
        
        uint16_t memoryAddress = eepromMemoryAddressForHour(currentProgram, currentHour);
        eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
        
        // Проверяем, что в памяти записано что-то осмысленное (не пустая стертая память)
        if (unTable.spProg.spT0on != -1 && (uint16_t)unTable.spProg.spT0on != 0xFFFF) {
            
            // Если параметры заслонки некорректны (больше 100%), значит в EEPROM старый мусор
            if (unTable.spProg.flapMin > 100 || unTable.spProg.flapMax > 100 || unTable.spProg.flapCurr > 100) {
                MYDEBUG_PRINTLN("EEPROM program values are invalid (>100%). Rewriting Program 1...");
                prepareProg(); // Перезаписываем дефолтом
                eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable)); // Перечитываем
            }

            settings.spT0on = unTable.spProg.spT0on;
            settings.spT0off = unTable.spProg.spT0off;
            settings.spT1on = unTable.spProg.spT1on;
            settings.spT1off = unTable.spProg.spT1off;
            
            // Безопасное присвоение с ограничением
            settings.minFlap = (unTable.spProg.flapMin <= 100) ? unTable.spProg.flapMin : 0;
            settings.maxFlap = (unTable.spProg.flapMax <= 100) ? unTable.spProg.flapMax : 100;
            settings.curFlap = (unTable.spProg.flapCurr <= 100) ? unTable.spProg.flapCurr : settings.minFlap;
            
            MYDEBUG_PRINT("Hourly program "); MYDEBUG_PRINT(currentProgram);
            MYDEBUG_PRINT(" applied for hour "); MYDEBUG_PRINTLN(currentHour);

            char logMsg[132];
            #if defined(LANG_RU)
            snprintf(logMsg, sizeof(logMsg), "Программа %u: Час %u. Уставки: T0=%.1f..%.1f, T1=%.1f..%.1f, Заслонка=%u%% (мин:%u%%, макс:%u%%)",
                     currentProgram, currentHour,
                     settings.spT0on / 10.0f, settings.spT0off / 10.0f,
                     settings.spT1on / 10.0f, settings.spT1off / 10.0f,
                     settings.curFlap, settings.minFlap, settings.maxFlap);
            #elif defined(LANG_UA)
            snprintf(logMsg, sizeof(logMsg), "Програма %u: Година %u. Уставки: T0=%.1f..%.1f, T1=%.1f..%.1f, Заслінка=%u%% (мін:%u%%, макс:%u%%)",
                     currentProgram, currentHour,
                     settings.spT0on / 10.0f, settings.spT0off / 10.0f,
                     settings.spT1on / 10.0f, settings.spT1off / 10.0f,
                     settings.curFlap, settings.minFlap, settings.maxFlap);
            #else // Default to English
            snprintf(logMsg, sizeof(logMsg), "Program %u: Hour %u applied. Settings: T0=%.1f..%.1f, T1=%.1f..%.1f, Flap=%u%% (min:%u%%, max:%u%%)",
                     currentProgram, currentHour,
                     settings.spT0on / 10.0f, settings.spT0off / 10.0f,
                     settings.spT1on / 10.0f, settings.spT1off / 10.0f,
                     settings.curFlap, settings.minFlap, settings.maxFlap);
            #endif
            sysLogger.log(logMsg);
        } else {
            MYDEBUG_PRINT("Hourly program "); MYDEBUG_PRINT(currentProgram);
            MYDEBUG_PRINT(" for hour "); MYDEBUG_PRINT(currentHour);
            MYDEBUG_PRINTLN(" is empty, not applied.");
        }
    }
}
