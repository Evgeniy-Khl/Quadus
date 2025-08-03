#include "main.h"
#include "my_settings.h"

// AsyncWebServer server(80);      // Create AsyncWebServer object on port 80
ESP8266WebServer server(80);
WiFiClientSecure client;
MyTelegramBot bot(botToken, client);

RTC_DS3231 rtc;                     // Создаем объект RTC для DS3231
DateTime curT;

OneWire oneWire(ONE_WIRE_BUS_PIN);  // Создаем экземпляр объекта OneWire для взаимодействия с шиной 1-Wire
DallasTemperature sensors(&oneWire);// Передаем ссылку на объект oneWire в конструктор DallasTemperature
LiquidCrystal_I2C lcd(0x20, 16, 2);// Set the LCD address to 0x27 for a 16 chars and 2 line display

byte writePCF8574(byte data);

TM1638 module(13, 14, 12);    // Создаем объект module для TM1638
void ledSet(void);

void setup(){
  #ifdef DEBUG
    Serial.begin(115200);               // Инициализация последовательного порта для отладки
  #endif
  //--------------------------------- initialize the LCD -----------------------------------
  lcd.begin();  // ВЛОЖЕН > Wire.begin() Инициализация I2C (SDA, SCL по умолчанию для ESP8266 - GPIO4, GPIO5)

  uint8_t temp = writePCF8574(0xFF);    // Установить все пины в LOW (если они используются как выходы)

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print(version);
  if(temp){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("ERROR PCF8574");
    delay(3000);
  }
  delay(1000);
  //----------------------------------- MOUNTING FS ----------------------------------------
  DEBUG_PRINTLN("mounting FS...");
  bool lFS = LittleFS.begin();
  if(lFS) {
    DEBUG_PRINTLN("mounted file system");
    //--------------------------------- clean LittleFS, for testing -----------------------
    // **Здесь вы можете разместить LittleFS.format();  но ОЧЕНЬ ВАЖНО ПОНИМАТЬ КОГДА ЭТО ДЕЛАТЬ!**
    // Например, вы можете отформатировать файловую систему только при первом запуске или при определенном условии.
    // **ВНИМАНИЕ: Раскомментирование следующей строки приведет к форматированию LittleFS при каждом запуске!**
    // Проверка и форматирование, если необходимо
    // if (LittleFS.format()) {
    //   DEBUG_PRINTLN("LittleFS formatted successfully");
    // } else {
    //   DEBUG_PRINTLN("Failed to format LittleFS");
    // }
    //-------------------------------------------------------
    temp = checkSetpoint();
    if(temp){
      lcd.clear();
      lcd.setCursor(0,0);
      myPrint(error_,sizeof(error_));
      lcd.print(temp);
      lcd.setCursor(0,1);
      lcd.print("setpoint.json");
      delay(3000);
    }
    temp = checkConfig();
    if(temp){
      lcd.clear();
      lcd.setCursor(0,0);
      myPrint(error_,sizeof(error_));
      lcd.print(temp);
      lcd.setCursor(0,1);
      lcd.print("config.json");
      delay(3000);
    }
  } else {
    DEBUG_PRINTLN("failed to mount FS");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("failed");
    lcd.setCursor(0,1);
    lcd.print("to mount FS");
    delay(3000);
  }
  // #ifdef DEBUG
  //   //---------------------- Получение информации о файловой системе
  //   FSInfo fs_info;
  //   LittleFS.info(fs_info);
  //   DEBUG_PRINTF("Total space: %u bytes\n", fs_info.totalBytes);
  //   DEBUG_PRINTF("Used space: %u bytes\n", fs_info.usedBytes);
  //   DEBUG_PRINTF("Free space: %u bytes\n", fs_info.totalBytes - fs_info.usedBytes);
  // #endif
  //---------------------------- инициализация WiFiManager -----------------------------------
  if(settings.special & 0x03) initWiFiManag();
  else DEBUG_PRINTLN("Запрет на подключение к WiFi! Продолжаем работу в оффлайн-режиме.");
  initEnvironment();
  //------------------------------------------------------------------------------------------
  // if(RTCENABLE) data[1] = NUMBER_FONT[1]; //"o1o ooo oo"
  digitalWrite(BEEP_PIN, HIGH); // Выключаем бипер
  pinMode(BEEP_PIN, OUTPUT);    // Настраиваем пин бипера как выход только для LED
  
  portOut.value = 0xFF;
  delay(3000);
  lcd.clear();
  displSwitch();
  // REACHED0 = 1; REACHED1 = 1;
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
        if(setupNum == 0) displSwitch(); else setupSwitch();
      } 
      else if(keys == 0) {waitCheckKeyPad = MINWAIT; keyCount = 0;}
      else lastKey = keys;
    }

  //============================= НОВАЯ ПОЛ-СЕКУНДА =================================
  if(now - counter1s > 500){
    counter1s = now; 
    if(resetDispl){
      if(--resetDispl == 0) {
        if(setupNum) saveSetPoint();
        setupNum = 0; // возврат к главному дисплею
        lcd.clear(); 
        displSwitch();
      }
    }
    alarm(0); alarm(1);
    //------------------------ новая минута --------------------------
    if(++halfSecond > 119){
      halfSecond = 0;
      if(setupNum == 0) displSwitch(); else setupSwitch();
      curT = rtc.now();
      time_t utc_time = rtc.now().unixtime();
      timeinfo = localtime(&utc_time);

    // Время, которое хранится в RTC (UTC)
      DEBUG_PRINT("DateTime class from DS3231 (UTC): ");
      DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                    curT.year(),
                    curT.month(),
                    curT.day(),
                    curT.hour(),
                    curT.minute(),
                    curT.second());

      // Время, сконвертированное для нашего часового пояса
      DEBUG_PRINT("Converted Local Time (EET/EEST): ");
      DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                    timeinfo->tm_year + 1900,
                    timeinfo->tm_mon + 1,
                    timeinfo->tm_mday,
                    timeinfo->tm_hour,
                    timeinfo->tm_min,
                    timeinfo->tm_sec);

      //---------------------------- новый час ----------------------------------
      if(++minutes > 59){
        minutes = 0;
        if(RTCENABLE){
          curT = rtc.now();                                             // Получаем текущее время с модуля DS3231
          if(WIFIENABLE){
            // ------------- Логика ежедневной синхронизации --------------
            if (curT.day() != lastSyncDay && curT.hour() == 3) {  // Проверяем, наступил ли новый день И сейчас 3 часа ночи
              DEBUG_PRINTLN("\nIt's 3 AM, time for daily sync!");
              syncTime();                                                     // Запускаем нашу функцию синхронизации
            }
            DEBUG_PRINTF("%04d/%02d/%02d %02d:%02d:%02d\n",                   // Выводим текущее время из RTC в монитор порта каждый час
                          curT.year(),curT.month(),curT.day(),
                          curT.hour(),curT.minute(),curT.second());
          }
        }
      } // --------------------------------------------------------------------
    } 
  }
}//============================================== END LOOP =============================================

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