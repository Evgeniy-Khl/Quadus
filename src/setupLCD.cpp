#include "setupLCD.h"

extern char displStr[16];

//---------- Ручное управление выходами --------------
void setup1(){
    int position = 3; // Текущая позиция для записи в строку
    displStr[0] = ' ';
    displStr[1] = ' ';
    displStr[2] = ' ';
    // 2. Проходим по массиву и собираем строку с разделителями
    for (int i = 0; i < 6; ++i) {
        char symbol;
        if (dataOut[i] == -1) symbol = 'A';
        else if (dataOut[i] == 0) symbol = '0';
        else symbol = '1';
        
        displStr[position++] = symbol;  // Добавляем преобразованный символ в строку
        // Добавляем ';' после каждого символа, КРОМЕ последнего
        if (i < 5) {
            displStr[position++] = ';';
        }
    }
    displStr[position] = '\0';          // 3. Завершаем строку нулевым символом
    lcd.setCursor(0,0);
    myPrint(manual_control, sizeof(manual_control));
    lcd.setCursor(0,1);
    lcd.print(displStr);
}

void setup2(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.spT0off;
        editBuff1 = settings.spT0on;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"t1 max =%3u\xDF\x43",editBuff0);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t1 min =%3u\xDF\x43",editBuff1);
    lcd.print(displStr);
}

void setup3(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.spT1off;
        editBuff1 = settings.spT1on;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"t2 max =%3u\xDF\x43",editBuff0);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t2 min =%3u\xDF\x43",editBuff1);
    lcd.print(displStr);
}

void setRelay(uint8_t item){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        switch (item){
            case 1: editBuff0 = settings.water0on; editBuff1 = settings.water0off; break;
            case 2: editBuff0 = settings.water1on; editBuff1 = settings.water1off; break;
            case 3: editBuff0 = settings.water2on; editBuff1 = settings.water2off; break;
        }
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"W%u on:%3u m.",item,editBuff0);
    if(modeOut[item] == 0) strcat(displStr,"\xDF\x43");
    else strcat(displStr,"\x78\xB3\xBB\x2E");
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(modeOut[item] == 0) switchTimeOff(item,editBuff1);
    else sprintf(displStr,"W%u off:%3u\xDF\x43",item,editBuff1);
    lcd.print(displStr);
}

void setLight(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.timerOn;
        editBuff1 = settings.timerOff;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"L on :%2u h.",editBuff0);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"L off:%2u h.",editBuff1);
    lcd.print(displStr);
}

void setAlarm(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.alarm0;
        editBuff1 = settings.alarm1;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"t1 alarm =%2u\xDF\x43",editBuff0);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t2 alarm =%2u\xDF\x43",editBuff1);
    lcd.print(displStr);
}

//---------- Ручное управление выходами --------------
void setModeOut(){
    int position = 4; // Текущая позиция для записи в строку
    displStr[0] = ' ';
    displStr[1] = ' ';
    displStr[2] = ' ';
    displStr[3] = ' ';
    // 2. Проходим по массиву и собираем строку с разделителями
    for (int i = 0; i < 5; ++i) {
        char symbol;
        if (modeOut[i] == 2) symbol = '2';
        else if (modeOut[i] == 0) symbol = '0';
        else symbol = '1';
        
        displStr[position++] = symbol;  // Добавляем преобразованный символ в строку
        // Добавляем ';' после каждого символа, КРОМЕ последнего
        if (i < 4) {
            displStr[position++] = ';';
        }
    }
    displStr[position] = '\0';          // 3. Завершаем строку нулевым символом
    lcd.setCursor(0,0);
    myPrint(output_mode, sizeof(output_mode));
    lcd.setCursor(0,1);
    lcd.print(displStr);
}

void setupSwitch(){
    switch (setupNum){
        case 1: setup1(); break;
        case 2: setup2(); break;
        case 3: setup3(); break;
        case 4: setRelay(1); break;
        case 5: setRelay(2); break;
        case 6: setRelay(3); break;
        case 7: setLight(); break;
        case 8: setAlarm(); break;
    }
}

// 1-1час.;2-2час.;3-3час.;4-4час.;5-6час.;6-8час.;7-10час.;8-12час.;9-24час.;10-2сут.;11-3сут.;12-4сут.;13-5сут.;14-6сут.;15-7сут.;
void switchTimeOff(uint8_t item, uint8_t point){
    uint8_t val = point;
    switch (point){
        case 5: val = 6; break;
        case 6: val = 8; break;
        case 7: val = 10; break;
        case 8: val = 12; break;
        case 9: val = 24; break;
        case 10: val = 2; break;
        case 11: val = 3; break;
        case 12: val = 4; break;
        case 13: val = 5; break;
        case 14: val = 6; break;
        case 15: val = 7; break;
    }
    if(point < 10) sprintf(displStr,"W%u off:%2u h.",item,val);
    else sprintf(displStr,"W%u off:%2u d.",item,val);
}

// void myPrint(const uint8_t* data, uint8_t size) {
//     for (size_t i = 0; i < size; i++) {
//         lcd.write(data[i]); // Выводим элемент по индексу i
//     }
// }