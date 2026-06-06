#include "setupLCD.h"

// Forward declarations for local functions
void setRelayMode(uint8_t item);
void switchTimeOff(uint8_t item, uint8_t val);

//---------- Ручное управление выходами --------------
void setup1(){
    int position = 3; // Текущая позиция для записи в строку
    displStr[0] = ' ';
    displStr[1] = ' ';
    displStr[2] = ' ';
    
    for (uint8_t i = 0; i < 6; i++) {
        if (dataOut[i] == -1) {
            displStr[position++] = '-';
        } else if (dataOut[i] == 0) {
            displStr[position++] = '0';
        } else {
            displStr[position++] = '1';
        }
    }
    displStr[position] = '\0'; // Завершаем строку
    
    lcd.setCursor(0,0);
    myPrint(wordSet,sizeof(wordSet));
    lcd.setCursor(0,1);
    myPrint(manual_control,sizeof(manual_control));
    lcd.setCursor(10,1);
    lcd.print(displStr);
}
// ------------------- setupNum=2 ------------------------------
void setup2(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.spT0on;
        editBuff1 = settings.spT0off;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),
        "A \x79\xB3\x69\xBC\xBA\xBD\x2E %2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));   //t1 увiмкн. ??.?°C
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        " \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8 %2d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));   //вимкнути ??.?°C
    lcd.print(displStr);
}
// ------------------- setupNum=3 ------------------------------
void setup3(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.spT1on;
        editBuff1 = settings.spT1off;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),
        "B \x79\xB3\x69\xBC\xBA\xBD\x2E %2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));   //t2 увiмкн. ??.?°C
    if(hasDHT22){
        displStr[0]  = '\xF4';  // #
        displStr[14] = '\x25';  // %
        displStr[15] = ' ';
    }
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        " \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8 %2d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));   //вимкнути. ??.?°C
    if(hasDHT22){
        displStr[0]  = '\xF4';  // #
        displStr[14] = '\x25';  // %
        displStr[15] = ' ';
    }
    lcd.print(displStr);
}
// ------------------- setupNum=4,5,6 ------------------------------
void setRelay(uint8_t item){
    uint8_t relMode = 0;
    if(NEWSCREEN){
        NEWSCREEN = 0;
        switch (item){
            case 1: editBuff0 = settings.water0on; editBuff1 = settings.water0off; relMode = settings.modeRelay1; break;
            case 2: editBuff0 = settings.water1on; editBuff1 = settings.water1off; relMode = settings.modeRelay2; break;
            case 3: editBuff0 = settings.water2on; editBuff1 = settings.water2off; relMode = settings.modeRelay3; break;
        }
    }
    lcd.setCursor(0,0);
    if(relMode == 0) {
        snprintf(displStr, sizeof(displStr),
            "T%u \x79\xB3\x69\xBC\xBA\xBD\x2E%3u\x78\xB3\x2E",item,editBuff0); //Tx увімкн.??хвл.
    } else {
        snprintf(displStr, sizeof(displStr),
            "T%u \x79\xB3\x69\xBC\xBA\x2E %2d.%d\xDF\x43",item, editBuff0 / 10, abs(editBuff0 % 10)); //Tx увімк.??.?°C
    }
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(relMode == 0) switchTimeOff(item,editBuff1);
    else snprintf(displStr, sizeof(displStr),
        " \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8 %2d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));    //вимкнути ??.?°C
    lcd.print(displStr);
}
// ------------------- setupNum=7 ------------------------------
void setLight(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.timerOn;
        editBuff1 = settings.timerOff;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),
        "CB \x79\xB3\x69\xBC\xBA\xBD\x2E%2u\xB4\x6F\xE3\x2E",editBuff0);   //CB увімкн.??год.
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        "CB \xB3\xB8\xBC\xBA\xBD\x2E %2u\xB4\x6F\xE3\x2E",editBuff1);      //CB вимкн. ??год.
    lcd.print(displStr);
}
// ------------------- setupNum=8 ------------------------------
void setAlarm(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.alarm0;
        editBuff1 = settings.alarm1;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),
        "A \xBF\x70\xB8\xB3\x6F\xB4\x61 =%d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));//t1 тривога =?.?°C
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        "B \xBF\x70\xB8\xB3\x6F\xB4\x61 =%d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));//t2 тривога =?.?°C
    lcd.print(displStr);
}
// ------------------- setupNum=12 ------------------------------
void setDevSpec(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.deviceNum;
        editBuff1 = settings.special;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),"\xA1\x6F\xBC\x65\x70 %3u",editBuff0);     //Номер ???
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),"\x43\xBF\x65\xC4 %3u",editBuff1);         //Спец ???
    lcd.print(displStr);
}
// ------------------- setupNum=11 ------------------------------
void setFlapProg(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.flap;
        editBuff1 = settings.program;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),"\xB7\x61\xC1\xBB\x2E %3u%%",editBuff0);    //Засл. ???%
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),"\x50\x70\x6F\xB3\x70\x2E %3u",editBuff1);  //Прогр. ???
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
        case 9: setRelayMode(1); break;
        case 10: setRelayMode(2); break;
        case 11: setFlapProg(); break;
        case 12: setDevSpec(); break;
        default: break;
    }
}
// ------------------- setupNum=9,10 ------------------------------
void setRelayMode(uint8_t item){
    uint8_t relMode = 0;
    if(item == 1) relMode = settings.modeRelay1; else relMode = settings.modeRelay2;
    lcd.setCursor(0,0);
    myPrint(wordSet,sizeof(wordSet));
    lcd.print(item);
    lcd.setCursor(0,1);
    myPrint(set_permissions,sizeof(set_permissions));
    lcd.print(relMode & 0x0F);
}

void switchTimeOff(uint8_t item, uint8_t val){
    snprintf(displStr, sizeof(displStr)," \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8 %3u\xE3\xB2\x2E",val);  //Rx вимкнути ??дб.
    lcd.print(displStr);
}
