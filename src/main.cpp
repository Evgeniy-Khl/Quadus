#include "main.h"
#include "my_settings.h"

ESP8266WebServer server(80);

RTC_DS3231 rtc;                             // Создаем объект RTC для DS3231

DHT dht(ONE_WIRE_BUS_PIN, DHT22);
OneWire oneWire(ONE_WIRE_BUS_PIN);          // Создаем экземпляр объекта OneWire для взаимодействия с шиной 1-Wire
DallasTemperature sensors(&oneWire);        // Передаем ссылку на объект oneWire в конструктор DallasTemperature
DeviceAddress sensorAddresses[MAX_DEVICE];  // Массив для хранения уникальных адресов датчиков

LiquidCrystal_I2C lcd(0x20, 16, 2);         // Set the LCD address to 0x27 for a 16 chars and 2 line display

byte writePCF8574(byte data);

TM1638 module(13, 14, 12);                  // Создаем объект module для TM1638
void ledSet(void);

void setup(){
  #ifdef DEBUG
    Serial.begin(115200);                   // Инициализация последовательного порта для отладки
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
  MYDEBUG_PRINTLN("mounting FS...");
  bool lFS = LittleFS.begin();
  if(lFS) {
    MYDEBUG_PRINTLN("mounted file system");
    //--------------------------------- clean LittleFS, for testing -----------------------
    // **Здесь вы можете разместить LittleFS.format();  но ОЧЕНЬ ВАЖНО ПОНИМАТЬ КОГДА ЭТО ДЕЛАТЬ!**
    // Например, вы можете отформатировать файловую систему только при первом запуске или при определенном условии.
    // **ВНИМАНИЕ: Раскомментирование следующей строки приведет к форматированию LittleFS при каждом запуске!**
    // Проверка и форматирование, если необходимо
    // if (LittleFS.format()) {
    //   MYDEBUG_PRINTLN("LittleFS formatted successfully");
    // } else {
    //   MYDEBUG_PRINTLN("Failed to format LittleFS");
    // }
    //-------------------------------------------------------
    temp = checkSetpoint();
    if(temp){
      lcd.clear();
      lcd.setCursor(0,0);
      myPrint(error_,sizeof(error_));// ПОМИЛКА 
      lcd.print(temp);
      lcd.setCursor(0,1);
      lcd.print("setpoint.json");
      delay(2000);
    }
  } else {
    MYDEBUG_PRINTLN("failed to mount FS");
    lcd.clear();
    lcd.setCursor(0,0);
    myPrint(error_,sizeof(error_)); lcd.print("- FS");// ПОМИЛКА - FS
    lcd.setCursor(0,1);
    myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect));// не підключено
    delay(3000);
  }
  //---------------------------- инициализация WiFiManager -----------------------------------
  if(settings.special & 0x03) initWiFiManag();
  else MYDEBUG_PRINTLN("Запрет на подключение к WiFi! Продолжаем работу в оффлайн-режиме.");
  initEnvironment();
  //----------------------- определяем какой датчик подключен --------------------------------
  sensorType();
  lcd.clear();
  lcd.setCursor(0,0);
  myPrint(sensorsWord,sizeof(sensorsWord));
  lcd.setCursor(0,1);
  switch (detectedSensor){
    case SENSOR_DS18B20:
        lcd.print("DS18B20: "); lcd.print(numberOfDevices); lcd.print("pcs.");
      break;
    case SENSOR_DHT22:
        lcd.print("DHT module.");
      break;
    case UNKNOWN:
        myPrint(no_,sizeof(no_)); myPrint(connect,sizeof(connect)); 
      break;
  }
  //------------------------------------------------------------------------------------------
  digitalWrite(BEEP_PIN, HIGH); // Выключаем бипер
  pinMode(BEEP_PIN, OUTPUT);    // Настраиваем пин бипера как выход только для LED
  
  delay(3000);  
  lcd.clear();
  displSwitch();
  portOut.value = 0xFF;
  if(RTCENABLE){
    relaySwitch(1);
    relaySwitch(2);
    relaySwitch(3);
    time_t utc_time = rtc.now().unixtime();
    timeinfo = localtime(&utc_time);
    uint8_t currentHour = timeinfo->tm_hour;// Получаем текущий час
    minutes = timeinfo->tm_min;             // Получаем текущуу минуту
    countSeconds = timeinfo->tm_sec;        // Получаем текущуу секунду
    if(checkLightState(currentHour, settings.timerOn, settings.timerOff)) LIGHT = PCF_ON; else LIGHT = PCF_OFF;
  }
}

void loop(){
	long now = millis();
  server.handleClient(); // Обработка входящих запросов
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
    halfSecond++; 
    if(resetDispl){
      if(--resetDispl == 0) {
        if(setupNum) saveSetPoint();
        setupNum = 0; // возврат к главному дисплею
        lcd.clear(); 
        displSwitch();
      }
    }
    if(halfSecond & 2){//-------- НОВАЯ СЕКУНДА -----------------------
      countSeconds++; errorsFlag.value = 0;
      sensorCheck();
      //--------------- температура -----------------------------------
      if(ds[0].pvT > 125) ERROR1 = 1;
      else HEATER = checkDeviceState(HEATER, ds[0].pvT, settings.spT0on, 35, settings.modeHeater);
      alarm(0);
      //----------------- влажность -----------------------------------
      if(ds[1].pvT > 125) ERROR2 = 1;
      else HUMIDI = checkDeviceState(HUMIDI, ds[1].pvT, 5, settings.spT1off, settings.modeHumidi);
      alarm(1);
      //---------------------------------------------------------------
      outStatusLed();
      if(setupNum == 0) displSwitch(); else setupSwitch();
    } //---------------------------------------------------------------
    if(halfSecond > 119){//------ новая минута ------------------------
      halfSecond = 0; countSeconds = 0; minutes++;
      if(RTCENABLE){
        // curT = rtc.now();
        time_t utc_time = rtc.now().unixtime();
        timeinfo = localtime(&utc_time);
        uint8_t currentHour = timeinfo->tm_hour;// Получаем текущий час
        minutes = timeinfo->tm_min;             // Получаем текущуу минуту
        countSeconds = timeinfo->tm_sec;        // Получаем текущуу секунду
        if(checkLightState(currentHour, settings.timerOn, settings.timerOff)) LIGHT = PCF_ON; else LIGHT = PCF_OFF;
        #ifdef DEBUG
        MYDEBUG_PRINTLN("checkLightState():");
        printBinary(portOut.value);
        #endif
        relaySwitch(1);
        relaySwitch(2);
        relaySwitch(3);
        /* // Время, которое хранится в RTC (UTC)
        MYDEBUG_PRINT("DateTime class from DS3231 (UTC): ");
        DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                      curT.year(), curT.month(),
                      curT.day(), curT.hour(),
                      curT.minute(), curT.second()); */

        // Время, сконвертированное для нашего часового пояса
        MYDEBUG_PRINT("Converted Local Time  (EET/EEST): ");
        DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                      timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                      timeinfo->tm_mday, timeinfo->tm_hour,
                      timeinfo->tm_min, timeinfo->tm_sec);
        uint16_t begHeapSize = ESP.getFreeHeap();    // Проверка доступной памяти
        DEBUG_PRINTF("Free heap size: %d\n", begHeapSize);
      }
      //---------------------------- новый час ----------------------------------
      if(++minutes > 59){
        minutes = 0;
        if(RTCENABLE){
          // time_t utc_time = rtc.now().unixtime();                             // Получаем текущее время с модуля DS3231
          // timeinfo = localtime(&utc_time);                                    // Преобразуем utc_time в структуру с локальным временем
          if(WIFIENABLE){
            // ------------- Логика ежедневной синхронизации --------------
            if (timeinfo->tm_mday != lastSyncDay && timeinfo->tm_hour == 15) { // Проверяем, наступил ли новый день. И сейчас 3 часа ночи
              MYDEBUG_PRINTLN("\nIt's 15:00 , time for daily sync!");
              syncTime();                                                     // Запускаем нашу функцию синхронизации
            }
            MYDEBUG_PRINT("Update Local Time  (EET/EEST): ");
            DEBUG_PRINTF("%04d-%02d-%02d %02d:%02d:%02d\n",
                      timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                      timeinfo->tm_mday, timeinfo->tm_hour,
                      timeinfo->tm_min, timeinfo->tm_sec);
          }
        }
      } // ------------------------- час ----------------------------
    } //--------------------------- минута --------------------------
  } //-------------------------- пол-секунда ------------------------
}//============================================== END LOOP =============================================

// Функция для записи байта на PCF8574
byte writePCF8574(byte data) {
  Wire.beginTransmission(PCF8574_ADDRESS);
  Wire.write(data);
  byte error = Wire.endTransmission();
  if (error == 0) {
    //MYDEBUG_PRINT("Data written: 0b");
    //printBinary(data);
    //MYDEBUG_PRINTLN();
  } else {
    MYDEBUG_PRINT("\nError writing to PCF8574. Error code: ");
    MYDEBUG_PRINTLN(error);
  }
  return error;
}