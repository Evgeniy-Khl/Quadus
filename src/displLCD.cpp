#include "displLCD.h"

char displStr[16];

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
      //***************************************************** */
    } else {
      lcd.setCursor(0,0);
      lcd.print(version);
      lcd.setCursor(0,1);
      sprintf(displStr,"sec:%3u; k=%3u",halfSecond,keys);
      lcd.print(displStr);
    }
}

void displ1(){
    int position = 0; // Текущая позиция для записи в строку
    // 2. Проходим по массиву и собираем строку с разделителями
    for (int i = 0; i < 6; ++i) {
        char symbol;
        if (dataOut[i] == -1) {
            symbol = 'A';
        } else if (dataOut[i] == 0) {
            symbol = '0';
        } else {
            symbol = '1';
        }
        displStr[position++] = symbol;  // Добавляем преобразованный символ в строку
        // Добавляем ';' после каждого символа, КРОМЕ последнего
        if (i < 5) {
            displStr[position++] = ';';
        }
    }
    displStr[position] = '\0';          // 3. Завершаем строку нулевым символом
    lcd.setCursor(0,0);
    lcd.print("Manual control");
    lcd.setCursor(0,1);
    lcd.print(displStr);
}

void displ2(){
    lcd.setCursor(0,0);
    sprintf(displStr,"t0on=%3u",settings.spT0on);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t0off=%3u",settings.spT0off);
    lcd.print(displStr);
}

void displ3(){
    lcd.setCursor(0,0);
    sprintf(displStr,"t1on=%3u",settings.spT1on);
    lcd.print(displStr);
    lcd.setCursor(0,1);
    sprintf(displStr,"t1off=%3u",settings.spT1off);
    lcd.print(displStr);
    
}
void displSwitch(){
    switch (displNum){
    case 0: displ0(); break;
    case 1: displ1(); break;
    case 2: displ2(); break;
    case 3: displ3(); break;
    
    default:       break;
    }
}