#include "displLCD.h"

char displStr[16];
//----------- Основной экран ------------------
void displ0(){
    if(WIFIENABLE){
      time_t now = time(nullptr);               // Получаем текущее время 
      struct tm* timeinfo = localtime(&now);    // Преобразуем его в структуру с локальным временем
      #ifdef DEBUG
        char buffer[80];                          // Буфер для форматированной строки времени
        strftime(buffer, sizeof(buffer), "%A, %B %d %Y %H:%M:%S", timeinfo);// Форматируем строку: "Понедельник, Июль 26 2024 15:02:15"
        // Выводим время в Монитор порта
        DEBUG_PRINT("Current time: ");
        DEBUG_PRINTLN(buffer);
      #endif
      lcd.setCursor(0,0);
      sprintf(displStr,"%2u.%2u.    %2u:%2u",timeinfo->tm_mday,timeinfo->tm_mon,timeinfo->tm_hour,timeinfo->tm_min);
      lcd.print(displStr);
    } else {
      lcd.setCursor(0,0);
      lcd.print(version);
      lcd.setCursor(0,1);
      sprintf(displStr,"min:%3u; k=%3u",minutes,keys);
      lcd.print(displStr);
    }
}
//---------- Ручное управление выходами --------------
void displ1(){
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
    lcd.print(" Manual control ");
    lcd.setCursor(0,1);
    lcd.print(displStr);
}

void displ2(){
    lcd.setCursor(0,0);
    sprintf(displStr,"t1 max =%3u C",settings.spT0off);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t1 min =%3u C",settings.spT0on);
    lcd.print(displStr);
}

void displ3(){
    lcd.setCursor(0,0);
    sprintf(displStr,"t2 max =%3u C",settings.spT1off);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t2 min =%3u C",settings.spT1on);
    lcd.print(displStr);
}

void displWater(uint8_t item){
    uint8_t valOn=0, valOff=0;
    switch (item){
        case 1: valOn = settings.water0on; valOff = settings.water0off; break;
        case 2: valOn = settings.water1on; valOff = settings.water1off; break;
        case 3: valOn = settings.water2on; valOff = settings.water2off; break;
    }
    lcd.setCursor(0,0);
    sprintf(displStr,"W%u on:%3u min",item,valOn);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    switchTimeOff(item,valOff);
    lcd.print(displStr);
}

void displLight(){
    lcd.setCursor(0,0);
    sprintf(displStr,"L on:%2u h.",settings.timerOn);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"L off:%2u h.",settings.timerOff);
    lcd.print(displStr);
}

void displAlarm(){
    lcd.setCursor(0,0);
    sprintf(displStr,"t1 alarm=%2u C",settings.alarm0);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t2 alarm=%2u C",settings.alarm1);
    lcd.print(displStr);
}

void displSwitch(){
    switch (displNum){
        case 0: displ0(); break;
        case 1: displ1(); break;
        case 2: displ2(); break;
        case 3: displ3(); break;
        case 4: displWater(1); break;
        case 5: displWater(2); break;
        case 6: displWater(3); break;
        case 7: displLight(); break;
        case 8: displAlarm(); break;
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