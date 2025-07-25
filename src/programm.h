#ifndef PROGRAMM_H
#define PROGRAMM_H
#include "main.h"

struct TableForOneHour{
    uint8_t spT0on; 	  // 0-120 Уставка температуры T0 ON
    uint8_t spT0off; 	  // 0-120 Уставка температуры T0 OFF
    uint8_t spT1on; 	  // 100-999 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
    uint8_t spT1off; 	  // 100-999 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
    uint8_t watering0;  // 0-60 Длительность включ.состояниe полива № 1
    uint8_t watering1;  // 0-60 Длительность включ.состояниe полива № 2
    uint8_t watering2;  // 0-60 Длительность включ.состояниe полива № 3
    uint8_t timerFlap;  // 0-100 Заслонка текущее положение маска 0x7F; ВКЛ./ОТКЛ. маска 0x80
};
//Programm 1: 0-3 -> p0;  4-7 -> p1;  8-11 -> p2;  12-15 -> p3;  16-19 -> p4;  20-23 -> p5;
//Programm 2: 0-3 -> p6;  4-7 -> p7;  8-11 -> p8;  12-15 -> p9;  16-19 -> p10; 20-23 -> p11;
//Programm 3: 0-3 -> p12; 4-7 -> p13; 8-11 -> p14; 12-15 -> p15; 16-19 -> p16; 20-23 -> p17;
union TableBuff {
    uint8_t buffer[8];
    struct TableForOneHour spHour;
};

extern TableBuff unBuf;

uint16_t eepromMemoryAddressForDay(uint8_t prg, uint8_t day);
byte eepromWrBuff(uint16_t memoryAddress, const uint8_t* buffer, uint8_t length);
void eepromRdBuff(uint16_t memoryAddress, uint8_t* buffer, uint8_t length);
void prepareTable(uint8_t prg, uint8_t day, uint8_t amountday, int16_t t0, int16_t t1, 
  int16_t rh, int16_t timer, int16_t aer0, int16_t aer1, int16_t fl);
void prepareProg1();
void prepareProg2();
void prepareProg3();
void prepareProg4();
void testProgs();

#endif /* PROGRAMM_H */