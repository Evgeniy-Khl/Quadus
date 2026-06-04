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

void setup2(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.spT0on;
        editBuff1 = settings.spT0off;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"t1 \x79\xB3\x69\xBC\xBA\xBD\x2E %2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));      //t1 увiмкн. ??.?°C
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t1 \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8%2d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));   //t1 вимкнути ??.?°C
    lcd.print(displStr);
}

void setup3(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.spT1on;
        editBuff1 = settings.spT1off;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"t2 \x79\xB3\x69\xBC\xBA\xBD\x2E %2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));      //t2 увiмкн. ??.?°C
    if(hasDHT22){
        displStr[0] = 'B';
        displStr[1] = 'o';
        sprintf(displStr + 11, "%2d.%d%% ", editBuff0 / 10, abs(editBuff0 % 10)); //Bo увiмкн. ??.?%
    }
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t2 \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8%2d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));   //t2 вимкнути ??.?°C
    if(hasDHT22){
        displStr[0] = 'B';
        displStr[1] = 'o';
        sprintf(displStr + 11, "%2d.%d%% ", editBuff1 / 10, abs(editBuff1 % 10)); //Bo вимкнути ??.?%
    }
    lcd.print(displStr);
}

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
        sprintf(displStr,"T%u \x79\xB3\x69\xBC\xBA\xBD\x2E%2u \x78\xB3\x2E",item,editBuff0); //Tx увімкн.??хвл.
    } else {
        sprintf(displStr,"T%u \x79\xB3\x69\xBC\xBA\xBD\x2E%2d.%d\xDF\x43",item, editBuff0 / 10, abs(editBuff0 % 10)); //Tx увімкн.??.?°C
    }
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(relMode == 0) switchTimeOff(item,editBuff1);
    else sprintf(displStr,"T%u \xB3\xB8\xBC\xBA\xBD\x2E%2d.%d\xDF\x43",item, editBuff1 / 10, abs(editBuff1 % 10));    //Tx вимкн.??.?°C
    lcd.print(displStr);
}

void setLight(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.timerOn;
        editBuff1 = settings.timerOff;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"CB \x79\xB3\x69\xBC\xBA\xBD\x2E%2u\xB4\x6F\xE3\x2E",editBuff0);   //CB увімкн.??год.
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"CB \xB3\xB8\xBC\xBA\xBD\x2E %2u\xB4\x6F\xE3\x2E",editBuff1);      //CB вимкн. ??год.
    lcd.print(displStr);
}

void setAlarm(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.alarm0;
        editBuff1 = settings.alarm1;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"t1 \xBF\x70\xB8\xB3\x6F\xB4\x61 =%2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));//t1 тривога =??.?°C
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t2 \xBF\x70\xB8\xB3\x6F\xB4\x61 =%2d.%d\xDF\x43", editBuff1 / 10, abs(editBuff1 % 10));//t2 тривога =??.?°C
    lcd.print(displStr);
}

void setDevSpec(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.deviceNum;
        editBuff1 = settings.special;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"\xA1\x6F\xBC\x65\x70 %3u",editBuff0);     //Номер ???
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"\x43\xBF\x65\xC4 %3u",editBuff1);         //Спец ???
    lcd.print(displStr);
}

void setFlapProg(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.flap;
        editBuff1 = settings.program;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"\xB7\x61\xC1\xBB\x2E %3u%%",editBuff0);    //Засл. ???%
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"\x50\x70\x6F\xB3\x70\x2E %3u",editBuff1);  //Прогр. ???
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
    sprintf(displStr,"T%u \xB3\xB8\xBC\xBA\xBD\x2E %2u\xE3\xB2\x2E",item,val);  //Rx вимкн. ??дiб.
    lcd.print(displStr);
}
