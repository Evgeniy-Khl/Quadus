#include "main.h"
#include "programm.h"

TableBuff unTable;

/**
 * AT24C32 имеет страницы по 32 байта. 
 * адреса страниц: 0-31, 32-63, ... 
 * 0xF00 - 0xFFF резерв 255 байт.
 */
uint16_t eepromMemoryAddressForHour(uint8_t prg, uint8_t hour){
	// из расчета одна страница на 1 день, 1 прог.=30 страниц всего 30*4=120 страниц плюс 8 страниц резерв.
    uint16_t addressPage = (hour) * 8 + (prg - 1) * (8 * 24);  
	return addressPage;
}

byte eepromWrBuff(uint16_t memoryAddress, const uint8_t* buffer, uint8_t length) {
  uint8_t currentBufferIndex = 0;
 
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);
    Wire.write((uint8_t)(memoryAddress >> 8));   // Старший байт адреса
    Wire.write((uint8_t)(memoryAddress & 0xFF)); // Младший байт адреса

    for (uint8_t i = 0; i < length; ++i) {
      Wire.write(buffer[currentBufferIndex + i]);
    }

    byte status = Wire.endTransmission();
    if (status != 0) {
      DEBUG_PRINT("I2C Write Error in buffer (addr "); DEBUG_PRINT(memoryAddress);
      DEBUG_PRINT("). Status: "); DEBUG_PRINTLN(status);
      // Прервать дальнейшую запись этого буфера, если есть ошибка
    }
    delay(EEPROM_WRITE_DELAY); // Ожидание завершения цикла записи страницы
    return status; 
}

/**
 * @brief Читает массив байт (буфер) из EEPROM.
 * @param memoryAddress Начальный 16-битный адрес ячейки памяти.
 * @param buffer Указатель на буфер для сохранения прочитанных данных.
 * @param length Количество байт для чтения.
 */
void eepromRdBuff(uint16_t memoryAddress, uint8_t* buffer, uint8_t length) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((uint8_t)(memoryAddress >> 8));
  Wire.write((uint8_t)(memoryAddress & 0xFF));
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, (int)length); // Запросить 'length' байт
  for (uint16_t i = 0; i < length; i++) {
    if (Wire.available()) {
      buffer[i] = Wire.read();
    } else {
      buffer[i] = 0; // В случае ошибки заполнить нулем
    }
  }
}

// ----- Функция для подготовки стандартной талбицы ----------
void prepareTable(uint8_t prg, uint8_t t0on, uint8_t t0off, uint8_t t1on, uint8_t t1off){
    uint8_t flp = 2;
    uint8_t tmr = 0;
    tmr |= flp << 6;
    unTable.spHour.spT0on = t0on;
    unTable.spHour.spT0off = t0off;
    unTable.spHour.spT1on = t1on;
    unTable.spHour.spT1off = t1off;
    unTable.spHour.water0run = 10;  // 0-120 мин. Длительность включ.состояниe полива
    unTable.spHour.water1run = 20;  // 0-120 мин. Длительность включ.состояниe полива
    unTable.spHour.water2run = 30;  // 0-120 мин. Длительность включ.состояниe полива
    unTable.spHour.timerFlap = tmr; // 0-60 Освещение маска 0x3F / 0-3 (0-0%; 1-30%; 2-60%; 3-100%) Заслонка текущее положение
    
    for (size_t i = 0; i < 24; i++){
      if(i <= 4 || i >= 22){  // ночь
        unTable.spHour.spT0on = t0on-5;
        unTable.spHour.spT0off = t0off-5;
        unTable.spHour.spT1on = t1on-5;
        unTable.spHour.spT1off = t1off-5;
        unTable.spHour.water0run = 0;  // 0-120 мин. Длительность включ.состояниe полива
        unTable.spHour.water1run = 0;  // 0-120 мин. Длительность включ.состояниe полива
        unTable.spHour.water2run = 0;  // 0-120 мин. Длительность включ.состояниe полива
        unTable.spHour.timerFlap = 0;  // 0-60 Освещение маска 0x3F / 0-3 (0-0%; 1-30%; 2-60%; 3-100%) Заслонка текущее положение
      }
      else if((i > 4 || i < 8) || (i > 20 || i < 22)){ // вечер утро
        unTable.spHour.spT0on = t0on-2;
        unTable.spHour.spT0off = t0off-2;
        unTable.spHour.spT1on = t1on-2;
        unTable.spHour.spT1off = t1off-2;
        flp = 1;
        tmr = 60;
        tmr |= flp << 6;
        unTable.spHour.timerFlap = tmr; // 0-60 Освещение маска 0x3F / 0-3 (0-0%; 1-30%; 2-60%; 3-100%) Заслонка текущее положение
      }
      uint16_t memoryAddress = eepromMemoryAddressForHour(prg, i);
      byte res = 0;//eepromWrBuff(memoryAddress, unTable.buffer, sizeof(unTable));
      DEBUG_PRINT("HOUR:"); DEBUG_PRINT(i); 
      DEBUG_PRINT("; ADD:"); DEBUG_PRINT(memoryAddress);
      DEBUG_PRINT("; RES:"); DEBUG_PRINTLN(res);
    }
    
}
void prepareProg1(){
    DEBUG_PRINTLN("PROGRAMM: 1");
    prepareTable(1,22,24,18,20);
}

void prepareProg2(){
    DEBUG_PRINTLN("PROGRAMM: 2");
}

void prepareProg3(){
    DEBUG_PRINTLN("PROGRAMM: 3");
}

void prepareProg4(){
    DEBUG_PRINTLN("PROGRAMM: 4");
}

void testProgs(){
  DEBUG_PRINTLN("AT24C32 EEPROM Test.");
  uint16_t memoryAddress = eepromMemoryAddressForHour(1, 0);
  eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
  if(unTable.spHour.spT0on == -1){
    prepareProg1();
    DEBUG_PRINTLN("ПЕРЕЗАПИСАНА PROG N1");
  } else DEBUG_PRINTLN("PROGRAMM N1 Ok");

  // memoryAddress = eepromMemoryAddressForHour(2, 0);
  // eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
  // if(unTable.spHour.spT0on == -1){
  //   prepareProg2();
  //   DEBUG_PRINTLN("ПЕРЕЗАПИСАНА PROG N2");
  // } else DEBUG_PRINTLN("PROGRAMM N2 Ok");

  // memoryAddress = eepromMemoryAddressForHour(3, 0);
  // eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
  // if(unTable.spHour.spT0on == -1){
  //   prepareProg3();
  //   DEBUG_PRINTLN("ПЕРЕЗАПИСАНА PROG N3");
  // } else DEBUG_PRINTLN("PROGRAMM N3 Ok");

  // memoryAddress = eepromMemoryAddressForHour(4, 0);
  // eepromRdBuff(memoryAddress, unTable.buffer, sizeof(unTable));
  // if(unTable.spHour.spT0on == -1){
  //   prepareProg4();
  //   DEBUG_PRINTLN("ПЕРЕЗАПИСАНА PROG N4");
  // } else DEBUG_PRINTLN("PROGRAMM N4 Ok");
}