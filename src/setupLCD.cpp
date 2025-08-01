#include "setupLCD.h"

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
    uint8_t mode = 0;
    if(NEWSCREEN){
        NEWSCREEN = 0;
        switch (item){
            case 1: editBuff0 = settings.water0on; editBuff1 = settings.water0off; mode = settings.modeOut0; break;
            case 2: editBuff0 = settings.water1on; editBuff1 = settings.water1off; mode = settings.modeOut1; break;
            case 3: editBuff0 = settings.water2on; editBuff1 = settings.water2off; mode = settings.modeOut2; break;
        }
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"R%u on :%3u",item,editBuff0);
    if(mode == 0) strcat(displStr,"\x78\xB3\x2E");
    else strcat(displStr,"\xDF\x43");
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(mode == 0) switchTimeOff(item,editBuff1);
    else sprintf(displStr,"R%u off:%3u\xDF\x43",item,editBuff1);
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

//---------- Режим выхода --------------
void setModeOut(){
    displStr[0] = 'R';
    displStr[1] = '1';
    displStr[2] = ':';
    switch (settings.modeOut0){
        case 1:  displStr[3] = 'H'; break;
        case 2:  displStr[3] = 'O'; break;
        default: displStr[3] = 'T'; break;
    }
    displStr[4] = ' ';
    displStr[5] = ' ';
    displStr[6] = 'R';
    displStr[7] = '2';
    displStr[8] = ':';
    switch (settings.modeOut1){
        case 1:  displStr[9] = 'H'; break;
        case 2:  displStr[9] = 'O'; break;
        default: displStr[9] = 'T'; break;
    }
    displStr[10] = ' ';
    displStr[11] = ' ';
    displStr[12] = 'R';
    displStr[13] = '3';
    displStr[14] = ':';
    switch (settings.modeOut2){
        case 1:  displStr[15] = 'H'; break;
        case 2:  displStr[15] = 'O'; break;
        default: displStr[15] = 'T'; break;
    }
    displStr[16] = '\0';

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
        case 9: setModeOut(); break;
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
    if(point < 10) sprintf(displStr,"R%u off:%2u\xB4\x6F\xE3.",item,val);
    else sprintf(displStr,"R%u off:%2u\xE3\x69\xB2",item,val);
}

// void myPrint(const uint8_t* data, uint8_t size) {
//     for (size_t i = 0; i < size; i++) {
//         lcd.write(data[i]); // Выводим элемент по индексу i
//     }
// }