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
    lcd.setCursor(0,0);
    lcd.print("displ1()");
    lcd.setCursor(0,1);
    sprintf(displStr,"min:%3u; k=%3u",minutes,keys);
    lcd.print(displStr);
}

void displ2(){
    lcd.setCursor(0,0);
    lcd.print("displ2()");
    lcd.setCursor(0,1);
    sprintf(displStr,"min:%3u; k=%3u",minutes,keys);
    lcd.print(displStr);
}

void displ3(){
    lcd.setCursor(0,0);
    lcd.print("displ3()");
    lcd.setCursor(0,1);
    sprintf(displStr,"min:%3u; k=%3u",minutes,keys);
    lcd.print(displStr);
}

void displ4(){
    lcd.setCursor(0,0);
    lcd.print("displ4()");
    lcd.setCursor(0,1);
    sprintf(displStr,"min:%3u; k=%3u",minutes,keys);
    lcd.print(displStr);
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