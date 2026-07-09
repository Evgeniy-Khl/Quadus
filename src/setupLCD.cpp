#include "setupLCD.h"

// Forward declarations for local functions
void setRelayMode();
void setClimatMode();
void switchTimeOff(uint8_t item, uint8_t val);

//---------- Ручное управление выходами --------------
// LIGHT|HEATER|HUMIDI|RELAY1|RELAY2|RELAY3
void setup1(){
    uint8_t smbl[3]={0x20,0x20,0x20};
    for (uint8_t i = 0; i < 3; i++) {
        if (dataOut[i] == -1) {
            smbl[i] = 'A';
        } else if (dataOut[i] == 0) {
            smbl[i] = '0';
        } else {
            smbl[i] = '1';
        }
    }
    snprintf(displStr, sizeof(displStr), "\x43\xB3:%c \x48\xB4:%c \xA4\xB3:%c", smbl[0], smbl[1], smbl[2]);
    lcd.setCursor(0,0);
    lcd.print(displStr);
    for (uint8_t i = 3; i < 6; i++) {
        if (dataOut[i] == -1) {
            smbl[i-3] = 'A';
        } else if (dataOut[i] == 0) {
            smbl[i-3] = '0';
        } else {
            smbl[i-3] = '1';
        }
    }
    snprintf(displStr, sizeof(displStr),"R1:%c R2:%c R3:%c",smbl[0],smbl[1],smbl[2]);
    lcd.setCursor(0,1);
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
        "t1 \x79\xB3\x69\xBC\xBA\xBD\x2E%2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));   //t1 увiмкн.??.?°C
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
        "t2 \x79\xB3\x69\xBC\xBA\xBD\x2E%2d.%d\xDF\x43", editBuff0 / 10, abs(editBuff0 % 10));   //t2 увiмкн.??.?°C
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
        "\x43\xB3 \x79\xB3\x69\xBC\xBA\xBD\x2E%2u\xB4\x6F\xE3\x2E",editBuff0);    //Cв увімкн.??год.
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        " \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8 %2u\xB4\x6F\xE3\x2E",editBuff1); // вимкнути ??год.
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

// ------------------- setupNum=9 modeRelay1, modeRelay2, modeRelay3 -
void setRelayMode(){
    lcd.setCursor(0,0);
    myPrint(settingUp, sizeof(settingUp));  // Налаштування
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        "R1=%d R2=%d R3=%d  ", settings.modeRelay1, settings.modeRelay2, settings.modeRelay3);
    lcd.print(displStr);
}

// ------------------- setupNum=10 modeHeater, modeHumidi, Прибор номер ?, Режим  WiFi  ? ------------
// Heater relay mode: 0=Heat, 1=Cool
// Humidifier relay mode: 0=Humidify, 1=Dehumidify
void setClimatMode(){
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),
        "\x4F\x78\x6F\xBB\x2E=%d \x4F\x63\x79\xC1\x2E=%d ", settings.modeHeater, settings.modeHumidi);// Охол.=x Осуш.=x
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),
        "ID=%d   WiFi = %d ", settings.deviceNum, settings.special);// ID=x   WiFi=x
    lcd.print(displStr);
}

// ------------------- setupNum=11 Заслонка MIN / MAX ------------------------------
void setFlopLimit(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.minFlap;
        editBuff1 = settings.maxFlap;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),"\xA4\x61\x63\xBB\x69\xBD\xBA\x61 MIN%3u",editBuff0);   //Заслiнка MIN ???
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),"\xA4\x61\x63\xBB\x69\xBD\xBA\x61 MAX%2u",editBuff1);   //Заслiнка MAX ???
    lcd.print(displStr);
}
// ------------------- setupNum=12 ------------------------------
void setFlapProg(){
    if(NEWSCREEN){
        NEWSCREEN = 0;
        editBuff0 = settings.curFlap;
        editBuff1 = settings.program;
    }
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr),"\xA4\x61\x63\xBB\x69\xBD\xBA\x61 %3u%%",editBuff0);   //Заслiнка ???%
    lcd.print(displStr);
    lcd.setCursor(0,1);
    snprintf(displStr, sizeof(displStr),"\xA8\x70\x6F\xB4\x70\x61\xBC\x61 \xCC%2u",editBuff1);  //Програма #???
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
        case 9: setRelayMode(); break;
        case 10: setClimatMode(); break;
        case 11: setFlopLimit(); break;
        case 12: setFlapProg(); break;
        default: break;
    }
}

void switchTimeOff(uint8_t item, uint8_t val){
    snprintf(displStr, sizeof(displStr)," \xB3\xB8\xBC\xBA\xBD\x79\xBF\xB8 %3u\xE3\xB2\x2E",val);  //Rx вимкнути ??дб.
    lcd.print(displStr);
}
