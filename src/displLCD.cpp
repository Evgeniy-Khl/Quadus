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
    } else {myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); lcd.print(" TC");}
    lcd.setCursor(0,1);
    if(WIFIENABLE) lcd.print(WiFi.localIP());
    else {myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); lcd.print(" WF");}
}
//---------- Температура датчиков и RH --------------
void displ1(){
    uint8_t permit;
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
        
            // permit = settings.modeHeater;
            // if(permit){
            //     if(LIGHT == PCF_OFF && permit == 2) permit = 0;
            //     else if(LIGHT == PCF_ON && permit == 1) permit = 0;
            // }
            // if(permit == 0){
            //     snprintf(displStr, sizeof(displStr),"\xD5%d.%d-%d.%d", 
            //              settings.spT0on / 10, abs(settings.spT0on % 10),
            //              settings.spT0off / 10, abs(settings.spT0off % 10));
            //     if(ERROR4) displStr[0] = '!';
            // } else {
            //     for (uint8_t i = 0; i < 8; i++){
            //         displStr[i] = ' ';
            //     }
            //     displStr[8] = '\0';
            // }
            if(ERROR4) lcd.print("ТРИВОГА!");   // MSG_ALARM_T1_RANGE
            else if(HEATER==PCF_ON) lcd.print("\x48\x61\xB4\x70\x69\xB3"); // Нагрiв
            else lcd.print("      ");
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
            snprintf(displStr, sizeof(displStr),"t2=%2d.%d", ds[1].pvT / 10, abs(ds[1].pvT % 10));
            if(hasDHT22){
                snprintf(displStr, sizeof(displStr), "Rh=%d.%d%% ", ds[1].pvT / 10, abs(ds[1].pvT % 10));
            }
            lcd.print(displStr);

            // permit = settings.modeHumidi;
            // if(permit){
            //     if(LIGHT == PCF_OFF && permit == 2) permit = 0;
            //     else if(LIGHT == PCF_ON && permit == 1) permit = 0;
            // }
            // if(permit == 0){
            //     snprintf(displStr, sizeof(displStr),"\xD5%d.%d-%d.%d", 
            //              settings.spT1on / 10, abs(settings.spT1on % 10),
            //              settings.spT1off / 10, abs(settings.spT1off % 10));
            //     if(ERROR8) displStr[0] = '!';
            // } else {
            //     for (uint8_t i = 0; i < 8; i++){
            //         displStr[i] = ' ';
            //     }
            //     displStr[8] = '\0';
            // }
            if(ERROR8) lcd.print("ТРИВОГА!");   // MSG_ALARM_T1_RANGE
            else if(HUMIDI==PCF_ON) lcd.print("\xA4\xB3\x6F\xBB\x6F\xB6\x2E"); // Зволож.
            else lcd.print("       ");
        }
    }
}
//---------- Остаток времени до переключения LT, R1 --------------
void displ2(){
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr)," CB %02u:%02u[%u\x2D%u]",timeinfo->tm_hour,timeinfo->tm_min, settings.timerOn, settings.timerOff);
    if(LIGHT == PCF_ON) displStr[0] = '\xDA'; // ↓
    else displStr[0] = '\xD9';      // ↑
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(pvTimeR1 == -1){
        lcd.print("T1 "); myPrint(no_permissions,sizeof(no_permissions));
    } else {
        if(RELAY1 == PCF_OFF){    //-- OFF --
            uint8_t day = pvTimeR1 / 1440;
            uint8_t hour = (pvTimeR1 % 1440) / 60;
            uint8_t min = pvTimeR1 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAT1 %u \xE3\xB2\x2E %02u:%02u ",day,hour,min);
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9T1 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR1);
        }
        lcd.print(displStr); 
    } 
}
//---------- Остаток времени до переключения R2, R3 --------------
void displ3(){
    lcd.setCursor(0,0);
    if(pvTimeR2 == -1){
        lcd.print("T2 "); myPrint(no_permissions,sizeof(no_permissions));
    } else {
        if(RELAY2 == PCF_OFF){    //-- OFF --
            uint8_t day = pvTimeR2 / 1440;
            uint8_t hour = (pvTimeR2 % 1440) / 60;
            uint8_t min = pvTimeR2 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAT2 %u \xE3\xB2\x2E %02u:%02u ",day,hour,min);
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9T2 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR2);
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
            snprintf(displStr, sizeof(displStr),"\xDAT3 %u \xE3\xB2\x2E %02u:%02u ",day,hour,min);
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9T3 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR3);
        }
        lcd.print(displStr);
    } 
}
//---------- Текушие ошибки --------------
void displ4(){
    lcd.setCursor(0,0);
    myPrint(error_,sizeof(error_));
    uint8_t er = errorsFlag.value;
    uint8_t x = 1, i, count = 0;
    for (i = 0; i < 8; i++){
        if(er & 1) {lcd.print(x); lcd.print(";"); if(++count == 4) break;}
        er >>= 1; x <<= 1;
    }
    lcd.setCursor(0,1);
    for (;i < 8; i++){
        er >>= 1; x <<= 1;
        if(er & 1) {lcd.print(x); lcd.print(";");}
    }
}

void displ5(){
    lcd.setCursor(0,0);
    lcd.print("displ5()");
    lcd.setCursor(0,1);
    sprintf(displStr,"min:%3u; k=%3u",minutes,keys);
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
