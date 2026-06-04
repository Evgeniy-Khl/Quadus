#ifndef _KEYPAD_H
#define _KEYPAD_H
#include "main.h"

#define KEY_1       1
#define KEY_2       16
#define KEY_3       2
#define KEY_4       32
#define KEY_5       4
#define KEY_5_4_6   KEY_5+KEY_4+KEY_6
#define KEY_6       64
#define KEY_6_1     KEY_6+KEY_1
#define KEY_6_2     KEY_6+KEY_2
#define KEY_6_3     KEY_6+KEY_3
#define KEY_6_4     KEY_6+KEY_4
#define KEY_6_5     KEY_6+KEY_5
#define KEY_6_5_1   KEY_6+KEY_5+KEY_1
#define KEY_6_5_2   KEY_6+KEY_5+KEY_2
#define KEY_6_5_3   KEY_6+KEY_5+KEY_3
#define KEY_6_5_4   KEY_6+KEY_5+KEY_4
#define KEY_7       8
#define KEY_7_1     KEY_7+KEY_1
#define KEY_7_2     KEY_7+KEY_2
#define KEY_7_2_4   KEY_7+KEY_2+KEY_4
#define KEY_7_3     KEY_7+KEY_3
#define KEY_7_4     KEY_7+KEY_4
#define KEY_7_4_6   KEY_7+KEY_4+KEY_6
#define KEY_7_4_6_8 KEY_7+KEY_4+KEY_6+KEY_8 // 232
#define KEY_7_5     KEY_7+KEY_5
#define KEY_7_6     KEY_7+KEY_6
#define KEY_7_8     KEY_7+KEY_7_8
#define KEY_8 128

void checkkey(uint8_t key);
void displIncr(void);
void displDecr(void);
int16_t incrVal(int16_t val, int16_t max);
int16_t decrVal(int16_t val, int16_t min);
void doSave();

#endif /*_KEYPAD_H*/
