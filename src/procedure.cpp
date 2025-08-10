#include "main.h"

void beeperOn(uint8_t val){
  beepOn = val;
  digitalWrite(BEEP_PIN, LOW); // Включаем бипер
}

// 1-1час.;2-2час.;3-3час.;4-4час.;5-6час.;6-8час.;7-10час.;8-12час.;9-24час.;10-2сут.;11-3сут.;12-4сут.;13-5сут.;14-6сут.;15-7сут.;
uint16_t transformTimeOff(uint8_t point){
    uint16_t val = point;
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
    if(point < 10) val *= 60;
    else val *= (60*24);
    return val; // max val = 10080 мин.
}

bool checkLightState(uint8_t currentHour, uint8_t onHour, uint8_t offHour) {
  if (onHour == offHour) return false;
  if (onHour < offHour) return (currentHour >= onHour && currentHour < offHour);
  else return (currentHour >= onHour || currentHour < offHour);
}

void relaySwitch(uint8_t cn){
  bool state = PCF_OFF, prnBit = false;
  int16_t val = 0, spOn = 0, spOff = 0, permit = 0;
  switch (cn){
    case 1: 
            state = RELAY1; 
            val = pvTimeR1; 
            spOn = settings.water0on; 
            spOff = settings.water0off;
            permit = settings.modeRelay1 & 3;
      break;
    case 2: 
            state = RELAY2; 
            val = pvTimeR2; 
            spOn = settings.water1on; 
            spOff = settings.water1off; 
            permit = settings.modeRelay2 & 3;
      break;
    case 3: 
            state = RELAY3; 
            val = pvTimeR3; 
            spOn = settings.water2on; 
            spOff = settings.water2off; 
            permit = settings.modeRelay3 & 3;
      break;
  }
  if(permit){ // если permission > 0 то ...
    if(LIGHT == PCF_OFF && permit == 2) permit = 0;// если permission == 2 разрешена работа только когда свет потушен
    else if(LIGHT == PCF_ON && permit == 1) permit = 0;// если permission == 1 разрешена работа только когда свет включен
  }

  if(permit == 0){// если permission == 0 разрешена работа всегдa
    spOff = transformTimeOff(spOff);
    if(--val <= 0){
      prnBit = true;
      if(state){//-- OFF --
        val = spOn; state = PCF_ON;
        MYDEBUG_PRINT("spOn="); MYDEBUG_PRINT(spOn);
        MYDEBUG_PRINT("; Relay:"); MYDEBUG_PRINT(cn); MYDEBUG_PRINTLN(" state = ON");
      } else {//-- ON --
        val = spOff; state = PCF_OFF;
        MYDEBUG_PRINT("spOff="); MYDEBUG_PRINT(spOff);
        MYDEBUG_PRINT("; Relay:"); MYDEBUG_PRINT(cn); MYDEBUG_PRINTLN(" state = OFF");
      }
    }
    switch (cn){
      case 1: RELAY1 = state; pvTimeR1 = val; break;
      case 2: RELAY2 = state; pvTimeR2 = val; break;
      case 3: RELAY3 = state; pvTimeR3 = val; break;
    }
    #ifdef DEBUG
    if(prnBit) printBinary(portOut.value);
    #endif
  } else {  // иначе отключаем
    switch (cn){
      case 1: RELAY1 = PCF_OFF; pvTimeR1 = -1; break;
      case 2: RELAY2 = PCF_OFF; pvTimeR2 = -1; break;
      case 3: RELAY3 = PCF_OFF; pvTimeR3 = -1; break;
    }
  }
}

bool checkDeviceState(bool previousState, int16_t currentTemp, int16_t onTemp, int16_t offTemp, uint8_t permit){
  if(permit){ // если permission > 0 то ...
    if(LIGHT == PCF_OFF && permit == 2) permit = 0;// если permission == 2 разрешена работа только когда свет потушен
    else if(LIGHT == PCF_ON && permit == 1) permit = 0;// если permission == 1 разрешена работа только когда свет включен
  }

  if(permit == 0){// если permission == 0 разрешена работа всегдa
    if (onTemp == offTemp) {
      return PCF_OFF;
    }
    if (onTemp < offTemp) { // Режим нагрева
      if (currentTemp <= onTemp) return PCF_ON;
      if (currentTemp >= offTemp) return PCF_OFF;
    } else { // Режим охлаждения
      if (currentTemp >= onTemp) return PCF_ON;
      if (currentTemp <= offTemp) return PCF_OFF;
    }
    return previousState; // В зоне гистерезиса состояние не меняем
  } else {
    return PCF_OFF;
  }
}

void outStatusLed(void){
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
        MYDEBUG_PRINTLN("Конфігурація не завантажена!");
        err = 1 ;
        saveSetPoint();  // значения по умолчанию
      }
  } else {
      saveSetPoint();  // значения по умолчанию
      MYDEBUG_PRINTLN("Конфігурація за замовчуванням!");
      err = 2 ;
  }
  
  sources = settings.modeRelay1 >> 4;         // маска 0x0F - разрешения реле 3; маска 0xF0 - источник реле 3
  MYDEBUG_PRINT("Source relay 1:"); MYDEBUG_PRINT(sources);
  sources |= settings.modeRelay2 & 0xF0;      // маска 0x0F - разрешения реле 4; маска 0xF0 - источник реле 4
  MYDEBUG_PRINT("; Source relay 2:"); MYDEBUG_PRINTLN(sources >> 4);
  MYDEBUG_PRINTLN("\n>> Итоговые значения после загрузки из FS:");
  #ifdef DEBUG
    printSetPoint();
  #endif
  return err;
}

uint8_t checkConfig(void){
  uint8_t err = 0;
  if(LittleFS.exists("/config.json")){
    //file exists, reading and loading
    MYDEBUG_PRINTLN("reading config file");
    File configFile = LittleFS.open("/config.json", "r");
    if(configFile){
      MYDEBUG_PRINTLN("opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      JsonDocument json;
      auto deserializeError = deserializeJson(json, buf.get());
      serializeJson(json, Serial);
      if( ! deserializeError ){
        MYDEBUG_PRINTLN("\nparsed json");
        strcpy(botToken, json["botToken"]);
        strcpy(chatID, json["chatID"]);
      } else {
        MYDEBUG_PRINTLN("failed to load json config");
        err = 3;
      }
      configFile.close();
    } else {
      MYDEBUG_PRINTLN("failed to open configuration file");
      err = 2;
    }
  } else {
    MYDEBUG_PRINTLN("does not exist config.json");
    err = 1;
  }
  return err;
}

//-------- Функция для печати текущих значений структуры в Serial порт --------
#ifdef DEBUG
void printSetPoint() {
    MYDEBUG_PRINTLN("--------------------");
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
    DEBUG_PRINTF("  modeLight: %d\n", settings.modeLight);
    DEBUG_PRINTF("  modeHeater: %d\n", settings.modeHeater);
    DEBUG_PRINTF("  modeHumidi: %d\n", settings.modeHumidi);
    DEBUG_PRINTF("  modeRelay1: %d\n", settings.modeRelay1);
    DEBUG_PRINTF("  modeRelay2: %d\n", settings.modeRelay2);
    DEBUG_PRINTF("  modeRelay3: %d\n", settings.modeRelay3);
    MYDEBUG_PRINTLN("--------------------");
}
#endif

//----------- Функция сохранения конфигурации в JSON файл ----------------
void saveSetPoint() {
    MYDEBUG_PRINTLN("Сохранение конфигурации...");
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
    obj["modeLight"] = settings.modeLight;
    obj["modeHeater"] = settings.modeHeater;
    obj["modeHumidi"] = settings.modeHumidi;
    obj["modeRelay1"] = settings.modeRelay1;
    obj["modeRelay2"] = settings.modeRelay2;
    obj["modeRelay3"] = settings.modeRelay3;

    lcd.clear();
    lcd.setCursor(0,0);
    // Открываем файл для записи
    File configFile = LittleFS.open("/setpoint.json", "w");
    if (!configFile) {
        MYDEBUG_PRINTLN("Не удалось открыть файл для записи");
        myPrint(file_damaged,sizeof(file_damaged));
        return;
    }
    myPrint(config,sizeof(config));
    lcd.setCursor(0,1);
    // Сериализуем JSON в файл
    if (serializeJson(doc, configFile) == 0) {
        MYDEBUG_PRINTLN("Ошибка записи в файл");
        myPrint(no_,sizeof(no_));
    } else {
        MYDEBUG_PRINTLN("Конфигурация успешно сохранена.");
        myPrint(saved,sizeof(saved));
    }
    delay(3000);
    lcd.clear();
    configFile.close();
}

//------------ Функция загрузки конфигурации из JSON файла -------------
bool loadSetPoint() {
    MYDEBUG_PRINTLN("Загрузка конфигурации...");

    // Открываем файл для чтения
    File configFile = LittleFS.open("/setpoint.json", "r");
    if (!configFile) {
        MYDEBUG_PRINTLN("Не удалось открыть файл для чтения. Используются значения по умолчанию.");
        return false;
    }

    // Создаем JSON документ для десериализации
    JsonDocument doc;

    // Десериализуем JSON из файла
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
        MYDEBUG_PRINT("Ошибка десериализации JSON: ");
        MYDEBUG_PRINTLN(error.c_str());
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
    settings.modeLight = obj["modeLight"];
    settings.modeHeater = obj["modeHeater"];
    settings.modeHumidi = obj["modeHumidi"];
    settings.modeRelay1 = obj["modeRelay1"];
    settings.modeRelay2 = obj["modeRelay2"];
    settings.modeRelay3 = obj["modeRelay3"];
  
    MYDEBUG_PRINTLN("Конфигурация успешно загружена.");
    return true;
}

#ifdef DEBUG
// Вспомогательная функция для вывода адреса датчика
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) MYDEBUG_PRINT("0");
    MYDEBUG_PRINT(deviceAddress[i], HEX);
    if (i < 7) MYDEBUG_PRINT(":");
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
*/
void alarm(uint8_t cn){
  int16_t val, maxVal, minVal, alarm;
  bool reched, beep = false;
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
    if(errorsFlag.value == 0x03) alarm = 100;
    else alarm = 50;
    if(disableBeep==0) beeperOn(alarm);// длительность звукового сигнала и включить канал 4 (6 А)
  }
  else disableBeep = 0;
}

// Вспомогательная функция для печати
void printBinary(unsigned char byte) {
  for (int i = 7; i >= 0; i--) {
    if(i > 5) {MYDEBUG_PRINT("x");}
    else {MYDEBUG_PRINT(bitRead(byte, i));}
  }
    MYDEBUG_PRINTLN("\n-----------------");
}

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
    settings.modeLight = 0,          // режим вывода реле;
    settings.modeHeater = 0,          // режим вывода реле;
    settings.modeHumidi = 0,          // режим вывода реле;
    settings.modeRelay1 = 0,          // режим вывода реле;
    settings.modeRelay2 = 0,          // режим вывода реле;
    settings.modeRelay3 = 0,          // режим вывода реле;

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
  /* MYDEBUG_PRINTLN("\n");
  uint32_t realSize = ESP.getFlashChipRealSize(); // Получаем реальный размер flash
  uint32_t ideSize = ESP.getFlashChipSize();    // Получаем размер, установленный в IDE
  FlashMode_t ideMode = ESP.getFlashChipMode();

  DEBUG_PRINTF("Flash real id:   %08X\n", ESP.getFlashChipId());
  DEBUG_PRINTF("Flash real size: %u bytes\n\n", realSize);

  DEBUG_PRINTF("Flash ide  size: %u bytes\n", ideSize);
  DEBUG_PRINTF("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
  DEBUG_PRINTF("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

  if (ideSize != realSize) {
    MYDEBUG_PRINTLN("Внимание! Размер Flash, установленный в IDE, не совпадает с реальным!");
  } else {
    MYDEBUG_PRINTLN("Размер Flash в IDE совпадает с реальным.");
  }
  MYDEBUG_PRINTLN(); */

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
        MYDEBUG_PRINTLN("RTC lost power, forcing initial time sync.");
        bool res = syncTime();        // true если неудалась синхронизация
        if(res){
          MYDEBUG_PRINTLN("НАСТРОЙКА времени в ручную.");
          manualTimeSet();
        }
    } else if(settings.special & 0x04){// перенастройка времени 
        settings.special &= 0xFB;
        saveSetPoint();
        bool res = syncTime();        // true если неудалась синхронизация
        if(res){
          MYDEBUG_PRINTLN("НАСТРОЙКА времени в ручную.");
          manualTimeSet();
        }
    } else {                          // если батарейка в порядке, функция вернёт false.
        MYDEBUG_PRINTLN("RTC has power, time should be valid.");
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
        MYDEBUG_PRINTLN("System time set from RTC. TZ rule applied.");
    }
    // Сохраняем текущий день, чтобы не синхронизироваться снова в этот же день
    lastSyncDay = rtc.now().day();
    //-----------ТЕСТ AT2432-------------------
    testProgs();              // тест
  } else MYDEBUG_PRINTLN("Couldn't find RTC!"); 
}

//------------ ФУНКЦИЯ СИНХРОНИЗАЦИИ ВРЕМЕНИ С NTP И ЗАПИСИ В RTC ------------
bool syncTime() {
  MYDEBUG_PRINTLN("\nStarting time synchronization...");
  // Конфигурируем и запускаем синхронизацию времени
  configTzTime(tzInfo, ntpServer);              // Новый, правильный метод
  MYDEBUG_PRINT("Waiting for NTP response");
  unsigned long startAttempt = millis();        // Засекаем время начала попытки
  lcd.clear();
  lcd.setCursor(0,0);
  while (time(nullptr) < 1000000000) {          // Ждём, пока время CPU не станет "большим"
    if (millis() - startAttempt > 10000) {      // Проверяем таймаут (например, 10 секунд)
      MYDEBUG_PRINTLN("\nFailed to obtain time (timeout).");
      return true;                                   // <-- ВЫХОД ИЗ ФУНКЦИИ по таймауту
    }
    MYDEBUG_PRINT(".");
    lcd.print("!");
    delay(1000);
  }
  // Этот код выполнится только при УСПЕШНОЙ синхронизации
  MYDEBUG_PRINTLN("\nTime successfully synchronized.");
  rtc.adjust(DateTime(time(nullptr)));
  MYDEBUG_PRINTLN("RTC time has been updated.");
  // lastSyncDay = rtc.now().day();
  return false;
}

// Функция для установки системного времени из RTC
// void setSystemTimeFromRTC() {
//   time_t t = rtc.now().unixtime();
//   timeval tv = {(long)t, 0};
//   settimeofday(==tv, nullptr);
// }

// Функция вывода текущего меню в Serial Port
void displayMenu(SetState state, const DateTime& dt){
  char buffer[40];
  switch (state) {
    case SET_YEAR:
      #ifdef DEBUG
      sprintf(buffer, "-> Установка ГОДА:   %04d", dt.year());
      #endif
      sprintf(displStr,"\x50\x69\xBA: %04d", dt.year());
      break;
    case SET_MONTH:
      #ifdef DEBUG
      sprintf(buffer, "-> Установка МЕСЯЦА: %02d", dt.month());
      #endif
      sprintf(displStr,"\x4D\x69\x63\xC7\xE5\xC4: %02d", dt.month());
      break;
    case SET_DAY:
      #ifdef DEBUG
      sprintf(buffer, "-> Установка ДНЯ:    %02d", dt.day());
      #endif
      sprintf(displStr,"\xE0\x65\xBD\xC4: %02d", dt.day());
      break;
    case SET_HOUR:
      #ifdef DEBUG
      sprintf(buffer, "-> Установка ЧАСА:   %02d", dt.hour());
      #endif
      sprintf(displStr,"\xA1\x6F\xE3\xB8\xBD\x61: %02d", dt.hour());
      break;
    case SET_MINUTE:
      #ifdef DEBUG
      sprintf(buffer, "-> Установка МИНУТ:  %02d", dt.minute());
      #endif
      sprintf(displStr,"\x58\xB3\xB8\xBB\xB8\xBD: %02d", dt.minute());
      break;
    case CONFIRM_SAVE:
      #ifdef DEBUG
      sprintf(buffer, "Сохранить это время?");
      #endif
      break;
  }
  MYDEBUG_PRINTLN(buffer);
  lcd.clear();
  lcd.setCursor(0,0);
  if(state == CONFIRM_SAVE) myPrint(save_time, sizeof(save_time));
  else lcd.print(displStr);
}

//------------------ ОСНОВНАЯ ФУНКЦИЯ РУЧНОЙ НАСТРОЙКИ ------------------------
void manualTimeSet(){
  DateTime tempTime = rtc.now();          // Начинаем с текущего времени из RTC
  SetState currentState = SET_YEAR;       // Начальное состояние - установка года
  MYDEBUG_PRINTLN("\n--- Вход в режим настройки времени ---");
  displayMenu(currentState, tempTime);  // Отображаем текущее меню
  while (true) {
    long now = millis();
    if(now - counterWait > waitCheckKeyPad){
      counterWait = now;
      keys = module.getButtons();             // Считываем состояние кнопок
      if(keys > 0){
        if (keys == KEY_6){                // Обработка кнопки "Выйти" в любой момент
          // rtc.adjust(tempTime);
          MYDEBUG_PRINTLN("Время не сохранено!");
          return; // Выходим из функции
        } else if(currentState == CONFIRM_SAVE){
          if (keys == KEY_3) {
              rtc.adjust(tempTime); // Установка нового времени
              lcd.setCursor(0,1);
              myPrint(time_saved,sizeof(time_saved));
              MYDEBUG_PRINTLN("Время сохранено!");
              delay(1000);
              return; // Выходим из функции
          }
        } else {
          keycheck(currentState, keys, tempTime);
          displayMenu(currentState, tempTime);  // Отображаем текущее меню
        }
      } else waitCheckKeyPad = MINWAIT;
    }
    delay(100);
  }
}

void keycheck(SetState& currentState, uint8_t key, DateTime& tempTime){
    waitCheckKeyPad = WAITCHECKKEYPAD/2;  // 0.5 сек. кнопка не доступна
    switch (currentState) {
      case SET_YEAR:
        if (keys == KEY_1) tempTime = tempTime + TimeSpan(365, 0, 0, 0);
        if (keys == KEY_2) tempTime = tempTime - TimeSpan(365, 0, 0, 0);
        if (keys == KEY_3) {currentState = SET_MONTH; waitCheckKeyPad = WAITCHECKKEYPAD * 5;}  // 5 сек. кнопка не доступна
        break;
      case SET_MONTH:
        if (keys == KEY_1) tempTime = tempTime + TimeSpan(30, 0, 0, 0); // Упрощенно
        if (keys == KEY_2) tempTime = tempTime - TimeSpan(30, 0, 0, 0); // Упрощенно
        if (keys == KEY_3) {currentState = SET_DAY; waitCheckKeyPad = WAITCHECKKEYPAD * 5;}  // 5 сек. кнопка не доступна
        break;
      case SET_DAY:
        if (keys == KEY_1) tempTime = tempTime + TimeSpan(1, 0, 0, 0);
        if (keys == KEY_2) tempTime = tempTime - TimeSpan(1, 0, 0, 0);
        if (keys == KEY_3) {currentState = SET_HOUR; waitCheckKeyPad = WAITCHECKKEYPAD * 5;}  // 5 сек. кнопка не доступна
        break;
      case SET_HOUR:
        if (keys == KEY_1) tempTime = tempTime + TimeSpan(0, 1, 0, 0);
        if (keys == KEY_2) tempTime = tempTime - TimeSpan(0, 1, 0, 0);
        if (keys == KEY_3) {currentState = SET_MINUTE; 
                            tempTime = tempTime - TimeSpan(0, 3, 0, 0);
                           waitCheckKeyPad = WAITCHECKKEYPAD * 5;  // 5 сек. кнопка не доступна
                           }
        break;
      case SET_MINUTE:
        if (keys == KEY_1) tempTime = tempTime + TimeSpan(0, 0, 1, 0);
        if (keys == KEY_2) tempTime = tempTime - TimeSpan(0, 0, 1, 0);
        if (keys == KEY_3) {currentState = CONFIRM_SAVE; waitCheckKeyPad = WAITCHECKKEYPAD * 5;}  // 5 сек. кнопка не доступна
        break;
      case CONFIRM_SAVE: break;
    }
}