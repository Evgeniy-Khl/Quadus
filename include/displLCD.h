#ifndef __DISPLAY_LCD_H
#define __DISPLAY_LCD_H

#include <main.h>
void displ0();
void displ1();
void displ2();
void displ3();
void displWater(uint8_t item);
void displLight();
void displAlarm();
void switchTimeOff(uint8_t item, uint8_t point);
void myPrint(const uint8_t* data, uint8_t size);

#endif /* __DISPLAY_LCD_H */
