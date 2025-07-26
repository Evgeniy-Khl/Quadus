#ifndef PROGRAMM_H
#define PROGRAMM_H
#include "main.h"

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