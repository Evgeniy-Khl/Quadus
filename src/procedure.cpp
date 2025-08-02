#include "main.h"

#define UNALTERED   2 // неизменный

void beeperOn(uint8_t val){
  beepOn = val;
  digitalWrite(BEEP_PIN, LOW); // Включаем бипер
}

//------------- симистричный таймер -------------------
// void rotate_trays(void){
//   if(!TURN){
//     if(--pvTimer == 0){
//       pvTimer = settings.timer; 
//       TURN = PCF_OFF;
//       DEBUG_PRINTLN("TURN = PCF_OFF");
//     }
//   } else {
//     if(--pvTimer == 0){
//       if(settings.timer) pvTimer = settings.timer;
//       else pvTimer = settings.timer;
//       TURN = PCF_ON;
//       DEBUG_PRINTLN("TURN = PCF_ON");
//     }
//   }
// }

//------------- индикация 66,0 - завис датчик. --------------
bool check_freeze(uint8_t i){
 if(ds[i].pvT == ds[i].previousValue){
    if(++ds[i].duration > 600){ds[i].duration = 600; return true;}
 } else {ds[i].duration = 0; ds[i].previousValue = ds[i].pvT;}
 return false;
}

uint8_t RelayPos(uint8_t val, uint8_t max, uint8_t min){
  uint8_t x = UNALTERED;
  if(val >= max) x = OFF;   // отключить
  if(val <= min) x = ON;    // включить
  return x;
}

uint8_t RelayNeg(uint8_t val, uint8_t max, uint8_t min){
  uint8_t x = UNALTERED;
  if(val >= max) x = ON;    // включить
  if(val <= min) x = OFF;   // отключить
  return x;
}

void OutStatusLed(void){
    for(uint8_t i = 0; i < 6; i++){
      uint8_t numBit = 1 << i;
      dataLed[i] = (~portOut.value) & numBit;
    }
}

uint8_t checkSetpoint(void){
  uint8_t err = 0;
  //--------- Загрузка конфигурации --------------------------------------------
  if(LittleFS.exists("/setpoint.json")){
      if(!loadSetPoint()){
        DEBUG_PRINTLN("Конфігурація не завантажена!");
        err = 1 ;
        saveSetPoint();  // значения по умолчанию
      }
  } else {
      saveSetPoint();  // значения по умолчанию
      DEBUG_PRINTLN("Конфігурація за замовчуванням!");
      err = 2 ;
  }
  DEBUG_PRINTLN("\n>> Итоговые значения после загрузки из FS:");
  #ifdef DEBUG
    printSetPoint();
  #endif
  return err;
}

uint8_t checkConfig(void){
  uint8_t err = 0;
  if(LittleFS.exists("/config.json")){
    //file exists, reading and loading
    DEBUG_PRINTLN("reading config file");
    File configFile = LittleFS.open("/config.json", "r");
    if(configFile){
      DEBUG_PRINTLN("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      JsonDocument json;
      auto deserializeError = deserializeJson(json, buf.get());
      serializeJson(json, Serial);
      if( ! deserializeError ){
        DEBUG_PRINTLN("\nparsed json");
        strcpy(botToken, json["botToken"]);
        strcpy(chatID, json["chatID"]);
      } else {
        DEBUG_PRINTLN("failed to load json config");
        err = 3;
      }
      configFile.close();
    } else {
      DEBUG_PRINTLN("failed to open configuration file");
      err = 2;
    }
  } else {
    DEBUG_PRINTLN("does not exist config.json");
    err = 1;
  }
  return err;
}

//-------- Функция для печати текущих значений структуры в Serial порт --------
#ifdef DEBUG
void printSetPoint() {
    DEBUG_PRINTLN("--------------------");
    DEBUG_PRINTF("  spT0on: %d\n", settings.spT0on);
    DEBUG_PRINTF("  spT0off: %d\n", settings.spT0off);
    DEBUG_PRINTF("  spT1on: %d\n", settings.spT1on);
    DEBUG_PRINTF("  spT1off: %d\n", settings.spT1off);
    DEBUG_PRINTF("  water0on: %d\n", settings.water0on);
    DEBUG_PRINTF("  water0off: %d\n", settings.water0off);
    DEBUG_PRINTF("  water1on: %d\n", settings.water1on);
    DEBUG_PRINTF("  water1off: %d\n", settings.water1off);
    DEBUG_PRINTF("  water2on: %d\n", settings.water2on);
    DEBUG_PRINTF("  water2off: %d\n", settings.water2off);
    DEBUG_PRINTF("  flap: %d\n", settings.flap);
    DEBUG_PRINTF("  timerOn: %d\n", settings.timerOn);
    DEBUG_PRINTF("  timerOff: %d\n", settings.timerOff);
    DEBUG_PRINTF("  alarm0: %d\n", settings.alarm0);
    DEBUG_PRINTF("  alarm1: %d\n", settings.alarm1);
    DEBUG_PRINTF("  special: %d\n", settings.special);
    DEBUG_PRINTF("  program: %d\n", settings.program);
    DEBUG_PRINTF("  modeOut0: %d\n", settings.modeOut0);
    DEBUG_PRINTF("  modeOut1: %d\n", settings.modeOut1);
    DEBUG_PRINTF("  modeOut2: %d\n", settings.modeOut2);
    DEBUG_PRINTLN("--------------------");
}
#endif

//----------- Функция сохранения конфигурации в JSON файл ----------------
void saveSetPoint() {
    DEBUG_PRINTLN("Сохранение конфигурации...");
    waitCheckKeyPad = WAITCHECKKEYPAD * 5;  // 5 сек. кнопка не доступна
    // Создаем JSON документ
    JsonDocument doc;

    // Создаем корневой JSON ОБЪЕКТ, а не массив
    JsonObject obj = doc.to<JsonObject>();

    // Добавляем данные в объект
    obj["spT0on"] = settings.spT0on;
    obj["spT0off"] = settings.spT0off;
    obj["spT1on"] = settings.spT1on;
    obj["spT1off"] = settings.spT1off;
    obj["water0on"] = settings.water0on;
    obj["water0off"] = settings.water0off;
    obj["water1on"] = settings.water1on;
    obj["water1off"] = settings.water1off;
    obj["water2on"] = settings.water2on;
    obj["water2off"] = settings.water2off;
    obj["flap"] = settings.flap;
    obj["timerOn"] = settings.timerOn;
    obj["timerOff"] = settings.timerOff;
    obj["alarm0"] = settings.alarm0;
    obj["alarm1"] = settings.alarm1;
    obj["special"] = settings.special;
    obj["deviceNum"] = settings.deviceNum;
    obj["program"] = settings.program;
    obj["modeOut0"] = settings.modeOut0;
    obj["modeOut1"] = settings.modeOut1;
    obj["modeOut2"] = settings.modeOut2;

    lcd.clear();
    lcd.setCursor(0,0);
    // Открываем файл для записи
    File configFile = LittleFS.open("/setpoint.json", "w");
    if (!configFile) {
        DEBUG_PRINTLN("Не удалось открыть файл для записи");
        myPrint(file_damaged,sizeof(file_damaged));
        return;
    }
    myPrint(config,sizeof(config));
    lcd.setCursor(0,1);
    // Сериализуем JSON в файл
    if (serializeJson(doc, configFile) == 0) {
        DEBUG_PRINTLN("Ошибка записи в файл");
        myPrint(no_,sizeof(no_));
    } else {
        DEBUG_PRINTLN("Конфигурация успешно сохранена.");
        myPrint(saved,sizeof(saved));
    }
    delay(3000);
    lcd.clear();
    configFile.close();
}

//------------ Функция загрузки конфигурации из JSON файла -------------
bool loadSetPoint() {
    DEBUG_PRINTLN("Загрузка конфигурации...");

    // Открываем файл для чтения
    File configFile = LittleFS.open("/setpoint.json", "r");
    if (!configFile) {
        DEBUG_PRINTLN("Не удалось открыть файл для чтения. Используются значения по умолчанию.");
        return false;
    }

    // Создаем JSON документ для десериализации
    JsonDocument doc;

    // Десериализуем JSON из файла
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
        DEBUG_PRINT("Ошибка десериализации JSON: ");
        DEBUG_PRINTLN(error.c_str());
        configFile.close();
        return false;
    }
// Закрываем файл после чтения
    configFile.close();

    // Получаем корневой JSON объект
    JsonObject obj = doc.as<JsonObject>();

    settings.spT0on = obj["spT0on"];
    settings.spT0off = obj["spT0off"];
    settings.spT1on = obj["spT1on"];
    settings.spT1off = obj["spT1off"];
    settings.water0on = obj["water0on"];
    settings.water0off = obj["water0off"];
    settings.water1on = obj["water1on"];
    settings.water1off = obj["water1off"];
    settings.water2on = obj["water2on"];
    settings.water2off = obj["water2off"];
    settings.flap = obj["flap"];
    settings.timerOn = obj["timerOn"];
    settings.timerOff = obj["timerOff"];
    settings.alarm0 = obj["alarm0"];
    settings.alarm1 = obj["alarm1"];
    settings.special = obj["special"];
    settings.deviceNum = obj["deviceNum"];
    settings.program = obj["program"];
    settings.modeOut0 = obj["modeOut0"];
    settings.modeOut1 = obj["modeOut1"];
    settings.modeOut2 = obj["modeOut2"];
  
    DEBUG_PRINTLN("Конфигурация успешно загружена.");
    return true;
}

#ifdef DEBUG
// Вспомогательная функция для вывода адреса датчика
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) DEBUG_PRINT("0");
    DEBUG_PRINT(deviceAddress[i], HEX);
    if (i < 7) DEBUG_PRINT(":");
  }
}
#endif

uint8_t tableRH(int16_t maxT, int16_t minT){
  int16_t dT = 255;
  if (maxT>199 && maxT<410){ // maxT> 19.9 и maxT< 41.0
     dT = (maxT-minT)*16/10;
     if (dT<0) dT = 240;        // задаем число при котором dT >>=3; выполняется -> dT>20
     maxT /=10;
     dT >>=3;
     if (dT>20) dT = 255;
     else if (dT==0) dT = 100;
     else {maxT -= 20; maxT *= 20; maxT += (dT-1); dT = tabRH[maxT];};
  }
  return dT;
}

/*
errors = 0x01   // ОШИБКА ДАТЧИКА 0  199-потерян; 66,0-завис [E01]
errors = 0x02   // ОШИБКА ДАТЧИКА 1  199-потерян; 66,0-завис [E02]
errors = 0x04   // ОТКЛОНЕНИЕ КАНАЛ 0 [E04]
errors = 0x08   // ОТКЛОНЕНИЕ КАНАЛ 1 [E08]
errors = 0x10   // отказ одного из двух датчиков температуры
errors = 0x20   // отказ вспомогательного датчика температуры
errors = 0x40   // ПЕРЕГРЕВ СИМИСТОРА ! [ПГ]
*/
void alarm(uint8_t cn){
  int16_t val, maxVal, minVal, alarm;
  bool reched, beep;
  if(cn){
    val = pvT1;
    maxVal = max(settings.spT1on, settings.spT1off);
    minVal = min(settings.spT1on, settings.spT1off);
    alarm = settings.alarm1;
    reched = REACHED1;
  } else {
    val = pvT0;
    maxVal = max(settings.spT0on, settings.spT0off);
    minVal = min(settings.spT0on, settings.spT0off);
    alarm = settings.alarm0;
    reched = REACHED0;
  }
  if(reched){
    if(val <= (minVal - alarm) || val >= (maxVal + alarm)) beep = true;
  }
  else if(val >= minVal && val <= maxVal) reched = true;      // вышли на заданную температуру
  if(cn){
    REACHED1 = reched;
    ERROR8 = beep;
  } else {
    REACHED0 = reched;
    ERROR4 = beep;
  }
  if(errorsFlag.value){
    if(errorsFlag.value & 0x03) alarm = 100;
    else alarm = 50;
    if(disableBeep==0) beeperOn(alarm);// длительность звукового сигнала и включить канал 4 (6 А)
  }
  else disableBeep = 0;
}

// // Вспомогательная функция для печати
// void printBinary(unsigned char byte) {
//   for (int i = 7; i >= 0; i--) {
//     DEBUG_PRINTLN(bitRead(byte, i));
//   }
// }

void reset(void){
    settings.spT0on = T0ON, 	      // 0-120 Уставка температуры T0 ON
    settings.spT0off = T0OFF, 	    // 0-120 Уставка температуры T0 OFF
    settings.spT1on = T1ON, 	      // 0-120 Уставка температуры T1 или 0-100 Уставка относительной влажности ON
    settings.spT1off = T1OFF, 	    // 0-120 Уставка температуры T1 или 0-100 Уставка относительной влажности OFF
    settings.water0on = WT0ON,      // 0-120 Длительность включ.состояниe полива № 1
    settings.water0off = WT0OFF,    // 0-60 Длительность отключ.состояниe полива № 1
    settings.water1on = WT1ON,      // 0-120 Длительность включ.состояниe полива № 2
    settings.water1off = WT1OFF,    // 0-60 Длительность отключ.состояниe полива № 2
    settings.water2on = WT2ON,      // 0-120 Длительность включ.состояниe полива № 3
    settings.water2off = WT2OFF,    // 0-60 Длительность отключ.состояниe полива № 3
    settings.flap = 0,              // 0-100 Заслонка текущее положение
    settings.timerOn = TIMERON,     // 0-24
    settings.timerOff = TIMEROFF,   // 0-24
    settings.alarm0 = ALARM0,       // 0-120 отклонение температуры T0
    settings.alarm1 = ALARM1,       // 0-120 отклонение температуры T1
    settings.special = 0,
    settings.deviceNum = 0,         // маска 0x0F - номер прибора; маска 0xF0 - версия;
    settings.program = 0,           // исполняемая программа;
    settings.modeOut0 = 2,          // режим вывода реле;
    settings.modeOut1 = 0,          // режим вывода реле;
    settings.modeOut2 = 0,          // режим вывода реле;

    lcd.clear();
    lcd.setCursor(0,0);
    myPrint(config,sizeof(config));
    lcd.setCursor(0,1);
    myPrint(restored,sizeof(restored));
    beeperOn(50);
    delay(500);
    beeperOn(50);
    delay(500);
    saveSetPoint();
    beeperOn(100);
    delay(3000);
}

//============================== Config ========================================
void initEnvironment(void){
// #ifdef DEBUG
//   char displStr[65];
// #endif
  
  //------------------------------------------------------------------------
  /* DEBUG_PRINTLN("\n");
  uint32_t realSize = ESP.getFlashChipRealSize(); // Получаем реальный размер flash
  uint32_t ideSize = ESP.getFlashChipSize();    // Получаем размер, установленный в IDE
  FlashMode_t ideMode = ESP.getFlashChipMode();

  DEBUG_PRINTF("Flash real id:   %08X\n", ESP.getFlashChipId());
  DEBUG_PRINTF("Flash real size: %u bytes\n\n", realSize);

  DEBUG_PRINTF("Flash ide  size: %u bytes\n", ideSize);
  DEBUG_PRINTF("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
  DEBUG_PRINTF("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

  if (ideSize != realSize) {
    DEBUG_PRINTLN("Внимание! Размер Flash, установленный в IDE, не совпадает с реальным!");
  } else {
    DEBUG_PRINTLN("Размер Flash в IDE совпадает с реальным.");
  }
  DEBUG_PRINTLN(); */

/* 
  //---------- Изменяем яркость светодиода ----------------------------------------
  // Пин, к которому подключен светодиод (GPIO2)
  pinMode(LEDPIN, OUTPUT);    // Устанавливаем пин светодиода как выход
  // Можно установить желаемую частоту ШИМ (опционально)
  // analogWriteFreq(1000);   // По умолчанию и так 1000 Гц
  // Можно установить желаемый диапазон (опционально)
  analogWriteRange(255);      // Если хотите диапазон 0-255
  //===============================================================================
 */

  
  // Wire.begin(D2, D1);      // Если вы хотите использовать другие пины для I2C (например, D2 для SDA, D1 для SCL)
  //--------------------- Инициализация PCF8574 ----------------------------------
  /* Пример: Установить все пины PCF8574 как выходы и выключить их (записать 0)
            Для PCF8574, чтобы использовать пин как "выход", мы просто записываем в него значение.
            Чтобы использовать пин как "вход", мы записываем в него '1' (высокий уровень),
            а затем читаем состояние. Внутренние подтягивающие резисторы слабые. 
  */

  //---------- Инициализация DS3231 ----------------------------------------
  if(rtc.begin()){
    RTCENABLE = 1;
    // Первичная синхронизация времени при запуске
    // Особенно важно, если у RTC села батарейка
    if (rtc.lostPower()) {            // батарейка села.
        DEBUG_PRINTLN("RTC lost power, forcing initial time sync.");
        syncTime();
    } else {                          // если батарейка в порядке, функция вернёт false.
        DEBUG_PRINTLN("RTC has power, time should be valid.");
        // 1. Устанавливаем системное время из RTC, чтобы оно было верным СРАЗУ
        // rtc.now(): Эта команда обращается к модулю DS3231 по шине I2C 
        // и считывает из него текущие данные: год, месяц, день, час, минуту и секунду. 
        // Она возвращает специальный объект типа DateTime.
        // .unixtime(): Это метод объекта DateTime, который конвертирует полученную дату и время в формат Unix-времени. 
        // Это одно большое число, представляющее собой количество секунд, прошедших с полуночи 1 января 1970 года по Гринвичу (UTC).
        time_t t = rtc.now().unixtime();      // сохраняем это число (Unix-время) в переменную t
        timeval tv = {(long)t, 0};            // специальная структура для этой функции, которая хранит секунды (tv_sec) и микросекунды (tv_usec).
        // создаем структуру tv, помещаем в неё наше время t (в секунды) и 0 (в микросекунды), а затем передаем её в settimeofday().
        settimeofday(&tv, nullptr);           // системная функция, которая устанавливает внутренние часы CPU.

        // 2. Включаем применение правил часового пояса И запускаем NTP в фоне
        // Это установит правило TZ и незаметно скорректирует время позже, если нужно
        configTime(tzInfo, ntpServer);
        Serial.println("System time set from RTC. TZ rule applied.");
    }
    // Сохраняем текущий день, чтобы не синхронизироваться снова в этот же день
    lastSyncDay = rtc.now().day();
    //-----------ТЕСТ AT2432-------------------
    testProgs();              // тест
  } else DEBUG_PRINTLN("Couldn't find RTC!"); 
  //==============================================================================

  //------------ Инициализация библиотеки DallasTemperature -----------------------------
  sensors.begin();
  sensors.setWaitForConversion(false);    // false: функция вернет управление немедленно.
  sensors.setCheckForConversion(false);   // Часто используется вместе с waitForConversion = false
  sensors.setAutoSaveScratchPad(false);   // Флаг автоматического сохранения настроек в EEPROM датчика.
  sensors.setResolution(12);// Устанавливаем разрешение для всех датчиков (9, 10, 11, or 12 бит)

  // Поиск устройств на шине 1-Wire
  numberOfDevices = sensors.getDeviceCount();
  if(numberOfDevices > MAX_DEVICE) numberOfDevices = MAX_DEVICE;
  DEBUG_PRINT("Found ");
  DEBUG_PRINT(numberOfDevices, DEC);
  DEBUG_PRINTLN(" devices.");
  
  #ifdef DEBUG
    if (numberOfDevices == 0) {
      DEBUG_PRINTLN("No DS18B20 sensors found! Check wiring and pull-up resistor.");
      // Можно остановить выполнение, если датчики не найдены
      // while(true) delay(100);
    } else {
      sensors.requestTemperatures(); // Отправляем команду на измерение
      DeviceAddress sensorAddress;
      DEBUG_PRINTLN("Sensor addresses:");
      // Выводим адрес каждого найденного устройства
      for (uint8_t i = 0; i < numberOfDevices; i++) {
        if (sensors.getAddress(sensorAddress, i)) {
          DEBUG_PRINT("  Sensor ");
          DEBUG_PRINT(i);
          DEBUG_PRINT(": ");
          printAddress(sensorAddress);
          DEBUG_PRINTLN();
        } else {
          DEBUG_PRINT("Could not get address for sensor ");
          DEBUG_PRINTLN(i);
        }
      }
    }
  #endif
  //==================================================================================
}

//------------ ФУНКЦИЯ СИНХРОНИЗАЦИИ ВРЕМЕНИ С NTP И ЗАПИСИ В RTC ------------
void syncTime() {
  DEBUG_PRINTLN("\nStarting time synchronization...");
  // Конфигурируем и запускаем синхронизацию времени
  configTzTime(tzInfo, ntpServer);              // Новый, правильный метод
  DEBUG_PRINT("Waiting for NTP response");
  unsigned long startAttempt = millis();        // Засекаем время начала попытки
  while (time(nullptr) < 1000000000) {          // Ждём, пока время CPU не станет "большим"
    if (millis() - startAttempt > 10000) {      // Проверяем таймаут (например, 10 секунд)
      DEBUG_PRINTLN("\nFailed to obtain time (timeout).");
      return;                                   // <-- ВЫХОД ИЗ ФУНКЦИИ по таймауту
    }
    DEBUG_PRINT(".");
    delay(1000);
  }
  // Этот код выполнится только при УСПЕШНОЙ синхронизации
  DEBUG_PRINTLN("\nTime successfully synchronized.");
  rtc.adjust(DateTime(time(nullptr)));
  DEBUG_PRINTLN("RTC time has been updated.");
  // lastSyncDay = rtc.now().day();
}

// Функция для установки системного времени из RTC
// void setSystemTimeFromRTC() {
//   time_t t = rtc.now().unixtime();
//   timeval tv = {(long)t, 0};
//   settimeofday(&tv, nullptr);
// }
