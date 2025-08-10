#include "displLCD.h"

//----------- ВРЕМЯ ДАТА и IP ------------------
void displ0(){
    lcd.setCursor(0,0);
    if(RTCENABLE){
    //   time_t unix_time = time(nullptr);             // Получаем текущее время из процессора, сконвертированное для нашего часового пояса
      time_t utc_time = rtc.now().unixtime();       // текущее время из DS3231 в формате Unix, сконвертированное для нашего часового пояса
    //   struct tm* timeUnix = localtime(&unix_time);  // Преобразуем unix_time в структуру с локальным временем
      timeinfo = localtime(&utc_time);    // Преобразуем utc_time в структуру с локальным временем
      sprintf(displStr,"%02u.%02u.%02u  %02u:%02u",timeinfo->tm_mday,timeinfo->tm_mon+1,
                        (timeinfo->tm_year+1900)%100,timeinfo->tm_hour,timeinfo->tm_min);
      lcd.print(displStr);
    } else {myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); lcd.print(" TC");}
    lcd.setCursor(0,1);
    if(WIFIENABLE) {lcd.print("IP "); lcd.print(WiFi.localIP());}
    else {myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); lcd.print(" WF");}
}
//---------- Температура датчиков и RH --------------
void displ1(){
    uint8_t permit;
    if(detectedSensor == UNKNOWN){
        lcd.setCursor(0,0);
        myPrint(sensorsWord,sizeof(sensorsWord));
        lcd.setCursor(0,1);
        myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect));
    } else {
        lcd.setCursor(0,0);
        if(ERROR1){
            lcd.print("t1 ");
            myPrint(error_,sizeof(error_));
            for (uint8_t i = 0; i < 5; i++){
                lcd.write(' ');
            }
        } else {
            snprintf(displStr, sizeof(displStr),"t1=%3u\xDF\x43",pvT0);            //t1=???°C ([8])
            lcd.print(displStr);
        
            permit = settings.modeHeater;
            if(permit){ // если permission > 0 то ...
                if(LIGHT == PCF_OFF && permit == 2) permit = 0;// если permission == 2 разрешена работа только когда свет потушен
                else if(LIGHT == PCF_ON && permit == 1) permit = 0;// если permission == 1 разрешена работа только когда свет включен
            }
            if(permit == 0){
                snprintf(displStr, sizeof(displStr)," [%2u\x2D%2u]", settings.spT0on, settings.spT0off);
                if(ERROR4) displStr[0] = '!';
            } else {
                for (uint8_t i = 0; i < 8; i++){
                    displStr[i] = ' ';
                }
                displStr[8] = '\0';
            }
            lcd.print(displStr);
        }
        //-------------------------------------------------------------------------------------------------------------
        lcd.setCursor(0,1);
        if(ERROR2){
            lcd.print("t2 ");
            myPrint(error_,sizeof(error_));
            for (uint8_t i = 0; i < 5; i++){
                lcd.write(' ');
            }
        } else {
            snprintf(displStr, sizeof(displStr),"t2=%3u\xDF\x43",pvT1);       //t2=???°C ([8])
            if(detectedSensor == DHT22){
                displStr[0] = 'B';
                displStr[1] = 'o';
                displStr[6] = '%';
                displStr[7] = ' ';     //Bo=???%
                displStr[8] = '\0';
            }
            lcd.print(displStr);

            permit = settings.modeHumidi;
            if(permit){ // если permission > 0 то ...
                if(LIGHT == PCF_OFF && permit == 2) permit = 0;// если permission == 2 разрешена работа только когда свет потушен
                else if(LIGHT == PCF_ON && permit == 1) permit = 0;// если permission == 1 разрешена работа только когда свет включен
            }
            if(permit == 0){
                snprintf(displStr, sizeof(displStr)," [%2u\x2D%2u]", settings.spT1on, settings.spT1off);
                if(ERROR8) displStr[0] = '!';
            } else {
                for (uint8_t i = 0; i < 8; i++){
                    displStr[i] = ' ';
                }
                displStr[8] = '\0';
            }
            lcd.print(displStr);
        }
    }
}
//---------- Остаток времени до переключения LT, R1 --------------
void displ2(){
    time_t utc_time = rtc.now().unixtime();       // текущее время из DS3231 в формате Unix, сконвертированное для нашего часового пояса
    timeinfo = localtime(&utc_time);    // Преобразуем utc_time в структуру с локальным временем
    lcd.setCursor(0,0);
    snprintf(displStr, sizeof(displStr)," CB %02u:%02u[%u\x2D%u]",timeinfo->tm_hour,timeinfo->tm_min, settings.timerOn, settings.timerOff);
    if(LIGHT) displStr[0] = '\xEE';
    else displStr[0] = '\xEF';
    lcd.print(displStr);
    lcd.setCursor(0,1);
    if(pvTimeR1 == -1){
        lcd.print("T1 "); myPrint(no_permissions,sizeof(no_permissions));
    } else {
        if(RELAY1){    //-- OFF --
            uint8_t day = pvTimeR1 / 1440;
            uint8_t hour = (pvTimeR1 % 1440) / 60;
            uint8_t min = pvTimeR1 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAT1 %u\xE3\x69\xB2\x2E %02u:%02u ",day,hour,min);       // Tx 0дiб. 00:00
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9T1 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR1); // Tx увiм.00хвл.
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
        if(RELAY2){    //-- OFF --
            uint8_t day = pvTimeR2 / 1440;
            uint8_t hour = (pvTimeR2 % 1440) / 60;
            uint8_t min = pvTimeR2 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAT2 %u\xE3\x69\xB2\x2E %02u:%02u ",day,hour,min);       // Tx 0дiб. 00:00
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9T2 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR2); // Tx увiм.00хвл.
        }
        lcd.print(displStr);
    } 
    lcd.setCursor(0,1);
    if(pvTimeR3 == -1){
        lcd.print("T3 "); myPrint(no_permissions,sizeof(no_permissions));
    } else {
        if(RELAY3){    //-- OFF --
            uint8_t day = pvTimeR3 / 1440;
            uint8_t hour = (pvTimeR3 % 1440) / 60;
            uint8_t min = pvTimeR3 % 60;
            snprintf(displStr, sizeof(displStr),"\xDAT3 %u\xE3\x69\xB2\x2E %02u:%02u ",day,hour,min);       // Tx 0дiб. 00:00
        } else {      //-- ON --
            snprintf(displStr, sizeof(displStr),"\xD9T3 \x79\xB3\x69\xBC\x2E%2u\x78\xB3\xBB\x2E",pvTimeR3); // Tx увiм.00хвл.
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
        lcd.write(data[i]); // Выводим элемент по индексу i
    }
}
