#ifndef __SETUP_LCD_H
#define __SETUP_LCD_H

#include <main.h>
void setup1();
void setup2();
void setRelay(uint8_t item);
void setLight();
void setAlarm();
void setModeOut();
void setupSwitch();
void switchTimeOff(uint8_t item, uint8_t point);
// void myPrint(const uint8_t* data, uint8_t size);

#endif /* __SETUP_LCD_H */
