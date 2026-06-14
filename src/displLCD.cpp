#include "displLCD.h"

//----------- ВРЕМЯ ДАТА и IP ------------------
void displ0(){
    lcd.setCursor(0,0);
    if(RTCENABLE){
      time_t utc_time = rtc.now().unixtime();
      timeinfo = localtime(&utc_time);
      sprintf(displStr,"%02u.%02u.%02u  %02u:%02u",timeinfo->tm_mday,timeinfo->tm_mon+1,
                        (timeinfo->tm_year+1900)%100,timeinfo->tm_hour,timeinfo->tm_min);
      lcd.print(displStr);
    } else {lcd.print("RTC "); myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect));}
    lcd.setCursor(0,1);
    if(WIFIENABLE) lcd.print(WiFi.localIP());
    else {lcd.print("WiFi "); myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect));}
}
//---------- Температура датчиков и RH --------------
void displ1(){
    char txt[10];
    if(!hasDHT22 && numberOfDS18 == 0){
        lcd.setCursor(0,0);
        myPrint(sensorsWord,sizeof(sensorsWord));
        lcd.setCursor(0,1);
        myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect));
    } else {
        lcd.setCursor(0,0);
        if(ERROR1){             // DEVICE_DISCONNECTED
            lcd.print("t1 ");
            myPrint(error_,sizeof(error_));
            for (uint8_t i = 0; i < 6; i++){
                lcd.write(' ');
            }
        } else {
            snprintf(displStr, sizeof(displStr),"t1=%2d.%d ", ds[0].pvT / 10, abs(ds[0].pvT % 10));
            lcd.print(displStr);
        
            if(settings.modeHeater == HEATER_MODE_HEAT){
                sprintf(txt,"\x48\x61\xB4\x70\x69\xB3\x20\x20"); // Нагрiв
            } else {
                sprintf(txt,"\x4F\x78\x6F\xBB\x6F\xE3\xB6\x2E"); // Охолодж.
            }
            lcd.setCursor(8,0);
            if(ERROR4) myPrint(alarm, sizeof(alarm));
            else if(HEATER==PCF_ON) lcd.print(txt);
            else lcd.print("\x20\x20\x20\x20\x20\x20\x20\x20");
        }
        //-------------------------------------------------------------------------------------------------------------
        lcd.setCursor(0,1);
        if(ERROR2){             // DEVICE_DISCONNECTED
            lcd.print("t2 ");
            myPrint(error_,sizeof(error_));
            for (uint8_t i = 0; i < 6; i++){
                lcd.write(' ');
            }
        } else {
            snprintf(displStr, sizeof(displStr),"t2=%2d.%d ", ds[1].pvT / 10, abs(ds[1].pvT % 10));
            if(hasDHT22){
                snprintf(displStr, sizeof(displStr), "%% =%2d.%d ", ds[1].pvT / 10, abs(ds[1].pvT % 10));
            }
            lcd.print(displStr);

            if(settings.modeHumidi == HUMIDI_MODE_HUMIDIFY){
                sprintf(txt,"\xA4\xB3\x6F\xBB\x6F\xB6\x2E\x20"); // Зволож.
            } else {
                sprintf(txt,"\x4F\x63\x79\xC1\x65\xBD\x2E\x20"); // Осушен.
            }
            lcd.setCursor(8,1);
            if(ERROR8) myPrint(alarm, sizeof(alarm));
            else if(HUMIDI==PCF_ON) lcd.print(txt); 
            else if(!hasDHT22){
                snprintf(txt, sizeof(txt),"Rh=%2d.%d%%", pvRH / 10, abs(pvRH % 10));
                lcd.print(txt);
            }
            else lcd.print("\x20\x20\x20\x20\x20\x20\x20\x20");
        }
    }
}
//---------- Остаток времени до переключения LT, R1 --------------
void displ2(){
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr)," CB %02u:%02u[%u\x2D%u]",timeinfo->tm_hour,timeinfo->tm_min, settings.timerOn, settings.timerOff);
    if(LIGHT == PCF_ON) displStr[0] = '\xD9'; // ↑
    else displStr[0] = '\xDA';      // ↓
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(settings.modeRelay1 == 0){
        if(RELAY1 == PCF_OFF){    //-- OFF --
            snprintf(displStr, sizeof(displStr),"\xDAR1 \xE0\x6F\xBE\x6F\xBC\x69\xB6\xBD\xB8\xB9");  // Допомiжний
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9R1 \xE0\x6F\xBE\x6F\xBC\x69\xB6\xBD\xB8\xB9");  // Допомiжний
        }
        lcd.print(displStr);
    } else {
        if(RELAY1 == PCF_OFF){    //-- OFF --
            uint8_t day = pvTimeR1 / 1440;
            uint8_t hour = (pvTimeR1 % 1440) / 60;
            uint8_t min = pvTimeR1 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAR1 %u \xE3\xB2\x2E %02u:%02u ",day,hour,min);
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9R1 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR1);
        }
        lcd.print(displStr); 
    } 
}
//---------- Остаток времени до переключения R2, R3 --------------
void displ3(){
    lcd.setCursor(0,0);
    if(settings.modeRelay2 == 0){
        if(RELAY2 == PCF_OFF){    //-- OFF --
            snprintf(displStr, sizeof(displStr),"\xDAR2 \xE0\x6F\xBE\x6F\xBC\x69\xB6\xBD\xB8\xB9");  // Допомiжний
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9R2 \xE0\x6F\xBE\x6F\xBC\x69\xB6\xBD\xB8\xB9");  // Допомiжний
        }
        lcd.print(displStr);
    } else {
        if(RELAY2 == PCF_OFF){    //-- OFF --
            uint8_t day = pvTimeR2 / 1440;
            uint8_t hour = (pvTimeR2 % 1440) / 60;
            uint8_t min = pvTimeR2 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAR2 %u \xE3\xB2\x2E %02u:%02u ",day,hour,min);
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9R2 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR2);
        }
        lcd.print(displStr);
    } 
    lcd.setCursor(0,1);
    if(pvTimeR3 == -1){
        lcd.print("T3 "); myPrint(no_permissions,sizeof(no_permissions));
    } else {
        if(RELAY3 == PCF_OFF){    //-- OFF --
            uint8_t day = pvTimeR3 / 1440;
            uint8_t hour = (pvTimeR3 % 1440) / 60;
            uint8_t min = pvTimeR3 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAR3 %u \xE3\xB2\x2E %02u:%02u ",day,hour,min);
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9R3 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR3);
        }
        lcd.print(displStr);
    } 
}
//---------- Статус выхода --------------
void displ4(){
    uint8_t smbl[3]={0x20,0x20,0x20}, port=0, out = sysState.portOut_m.value;
    for (uint8_t i = 0; i < 3; i++) {
        port = 1<<i;
        if (out & port) {
            smbl[i] = '\xDA';
        } else {
            smbl[i] = '\xD9';
        }
    }
    snprintf(displStr, sizeof(displStr), "\x43\xB3:%c \x48\xB4:%c \xA4\xB3:%c", smbl[0], smbl[1], smbl[2]);
    lcd.setCursor(0,0);
    lcd.print(displStr);
    for (uint8_t i = 3; i < 6; i++) {
        port = 1<<i;
        if (out & port) {
            smbl[i-3] = '\xDA';
        } else {
            smbl[i-3] = '\xD9';
        }
    }
    snprintf(displStr, sizeof(displStr),"R1:%c R2:%c R3:%c",smbl[0],smbl[1],smbl[2]);
    lcd.setCursor(0,1);
    lcd.print(displStr);
}
//---------- Уставки температуры --------------
void displ5(){
    lcd.setCursor(0,0);
    lcd.print("t1 ");
    snprintf(displStr, sizeof(displStr),"[%d.%d-%d.%d]", 
                         settings.spT0on / 10, abs(settings.spT0on % 10),
                         settings.spT0off / 10, abs(settings.spT0off % 10));
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(hasDHT22) lcd.print("%  "); else lcd.print("t2 ");
    snprintf(displStr, sizeof(displStr),"[%d.%d-%d.%d]", 
                         settings.spT1on / 10, abs(settings.spT1on % 10),
                         settings.spT1off / 10, abs(settings.spT1off % 10));
    lcd.print(displStr);
}

void displSwitch(){
    switch (displNum){
        case 1: displ1(); break;
        case 2: displ2(); break;
        case 3: displ3(); break;
        case 4: displ4(); break;
        case 5: displ5(); break;
        default: displ0(); break;
    }
}

void myPrint(const uint8_t* data, uint8_t size) {
    for (size_t i = 0; i < size; i++) {
        lcd.write(data[i]);
    }
}
