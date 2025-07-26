#include "main.h"
#include "my_settings.h"

// AsyncWebServer server(80);      // Create AsyncWebServer object on port 80
ESP8266WebServer server(80);
WiFiClientSecure client;
MyTelegramBot bot(botToken, client);

RTC_DS3231 rtc;                     // Создаем объект RTC для DS3231

OneWire oneWire(ONE_WIRE_BUS_PIN);  // Создаем экземпляр объекта OneWire для взаимодействия с шиной 1-Wire
DallasTemperature sensors(&oneWire);// Передаем ссылку на объект oneWire в конструктор DallasTemperature

byte writePCF8574(byte data);

TM1638 module(13, 14, 12);    // Создаем объект module для TM1638
  // void ledDisplKeypad(long now);
  void ledSet(void);

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x20, 16, 2);

void setup(){
  #ifdef DEBUG
    Serial.begin(115200);               // Инициализация последовательного порта для отладки
  #endif
  // initialize the LCD
  lcd.begin();  // ВЛОЖЕН > Wire.begin() Инициализация I2C (SDA, SCL по умолчанию для ESP8266 - GPIO4, GPIO5)

  uint8_t temp = writePCF8574(0xFF);    // Установить все пины в LOW (если они используются как выходы)

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("QUADUS v.0.0");
  if(temp){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERROR PCF8574");
    delay(3000);
  }
  delay(3000);
  //----------------------------------- MOUNTING FS ----------------------------------------
  DEBUG_PRINTLN("mounting FS...");
  bool lFS = LittleFS.begin();
  if(lFS) {
    DEBUG_PRINTLN("mounted file system");
    temp = checkSetpoint();
    if(temp){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ERROR");
      lcd.setCursor(0,1);
      lcd.print("setpoint.json");
      delay(3000);
    }
    temp = checkConfig();
    if(temp){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ERROR");
      lcd.setCursor(0,1);
      lcd.print("config.json");
      delay(3000);
    }
  } else {
    DEBUG_PRINTLN("failed to mount FS");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FAILED");
    lcd.setCursor(0,1);
    lcd.print("to mount FS");
    delay(3000);
  }
  #ifdef DEBUG
    //---------------------- Получение информации о файловой системе
    FSInfo fs_info;
    LittleFS.info(fs_info);
    DEBUG_PRINTF("Total space: %u bytes\n", fs_info.totalBytes);
    DEBUG_PRINTF("Used space: %u bytes\n", fs_info.usedBytes);
    DEBUG_PRINTF("Free space: %u bytes\n", fs_info.totalBytes - fs_info.usedBytes);
  #endif
  //---------------------------- инициализация WiFiManager -----------------------------------
  if(settings.sp_structs[0].special) initWiFiManag();
  else DEBUG_PRINTLN("Запрет на подключение к WiFi! Продолжаем работу в оффлайн-режиме.");
  initEnvironment();
  //------------------------------------------------------------------------------------------
  #ifdef LED_DISPLAY
    // if(RTCENABLE) data[1] = NUMBER_FONT[1]; //"o1o ooo oo"
    digitalWrite(BEEP_PIN, HIGH); // Выключаем бипер
    pinMode(BEEP_PIN, OUTPUT);    // Настраиваем пин бипера как выход только для LED
    // displ_IP();
  #else

  #endif
  pvTimer = settings.sp_structs[0].timer;                  // инициализация времени выключенного состояния таймера
  pvAeration = settings.sp_structs[0].aeration;            // инициализация ПАУЗы ПРОВЕТРИВАНИЯ (минут)
 
  portOut.value = 0xFF;
  delay(3000);

}

void loop(){
	long now = millis();
  //-------------------------------------------- 10 mSec. --------------------------------------
  if(now - counter10 > 10){
    counter10 = now;

    if(beepOn) beepOn--; else digitalWrite(BEEP_PIN, HIGH);   // Выключаем бипер

    keys = module.getButtons();
    if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}  // если не удерживается ни одна кнопка то сброс времени ожидания.
  }
  //-------------------------------------------- КЛАВИАТУРА --------------------------------------
    if(now - counterWait > waitCheckKeyPad){
      counterWait = now;
      keys = module.getButtons();
      
      if(lastKey == keys && keys > 0){
        keyCount++;
        checkkey(keys);
        // if(numSetup == 0) ledDispl(displNum);
        // else display_setup();
      } 
      else if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}
      else lastKey = keys;
    }

  //============================= НОВАЯ ПОЛ-СЕКУНДА =================================
  if(now - counter1s > 500){
    counter1s = now; 
    if(++halfSecond > 119) halfSecond = 0;
    // uint8_t temp = writePCF8574(halfSecond & 1);
    // DEBUG_PRINT("temp="); DEBUG_PRINTLN(temp);
    if(halfSecond == 0){
      //**************** Получаем текущее время ************** */
      time_t now = time(nullptr);
      // Преобразуем его в структуру с локальным временем
      struct tm* timeinfo = localtime(&now);
      // Буфер для форматированной строки времени
      char buffer[80];
      // Форматируем строку: "Понедельник, Июль 26 2024 15:02:15"
      strftime(buffer, sizeof(buffer), "%A, %B %d %Y %H:%M:%S", timeinfo);
      // Выводим время в Монитор порта
      DEBUG_PRINT("Current time: ");
      DEBUG_PRINTLN(buffer);

      lcd.setCursor(0,1);
      sprintf(displStr,"%2ud.%2u:%2u:%2u",timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
      lcd.print(displStr);
      //***************************************************** */
    }
  }
}

// Функция для записи байта на PCF8574
byte writePCF8574(byte data) {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(data);
  byte error = Wire.endTransmission();
  if (error == 0) {
    //DEBUG_PRINT("Data written: 0b");
    //printBinary(data);
    //DEBUG_PRINTLN();
  } else {
    DEBUG_PRINT("\nError writing to PCF8574. Error code: ");
    DEBUG_PRINTLN(error);
  }
  return error;
}