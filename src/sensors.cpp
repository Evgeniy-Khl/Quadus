#include "main.h"

#define TUNING	170

void sensorType(){
  MYDEBUG_PRINTLN("Определение типа датчика...");
  // 1. Пытаемся найти датчик DS18B20. Это более надежная проверка.
  sensors.begin(); // Инициализируем шину 1-Wire
  numberOfDevices = sensors.getDeviceCount();
  if(numberOfDevices > 0) {
      detectedSensor = SENSOR_DS18B20;
      if(numberOfDevices > MAX_DEVICE) numberOfDevices = MAX_DEVICE;
      MYDEBUG_PRINT("Обнаружен датчик DS18B20:"); MYDEBUG_PRINT(numberOfDevices, DEC); MYDEBUG_PRINTLN(" шт.");
      sensors.setWaitForConversion(false);    // false: функция вернет управление немедленно.
      sensors.setCheckForConversion(false);   // Часто используется вместе с waitForConversion = false
      sensors.setAutoSaveScratchPad(false);   // Флаг автоматического сохранения настроек в EEPROM датчика.
      sensors.setResolution(12);// Устанавливаем разрешение для всех датчиков (9, 10, 11, or 12 бит)
      sensors.requestTemperatures(); // Отправляем команду на измерение
      #ifdef DEBUG
        DeviceAddress sensorAddress;
        MYDEBUG_PRINTLN("Sensor addresses:");
        // Выводим адрес каждого найденного устройства
        for (uint8_t i = 0; i < numberOfDevices; i++) {
          if (sensors.getAddress(sensorAddress, i)) {
            MYDEBUG_PRINT("  Sensor ");
            MYDEBUG_PRINT(i);
            MYDEBUG_PRINT(": ");
            printAddress(sensorAddress);
            MYDEBUG_PRINTLN();
          } else {
            MYDEBUG_PRINT("Could not get address for sensor ");
            MYDEBUG_PRINTLN(i);
          }
        }
      #endif
   } else {
      // 2. Если DS18B20 не найден, пытаемся прочитать данные с DHT22.
      dht.begin(); // Инициализируем датчик DHT
      // Делаем тестовое чтение. Если результат не "NaN", значит, это DHT.
      if (!isnan(dht.readTemperature())) {
        detectedSensor = SENSOR_DHT22;
        MYDEBUG_PRINTLN("Обнаружен датчик: DHT22");
      }
   }
}

void sensorCheck(){
  switch (detectedSensor){
    case SENSOR_DHT22:{ // <--- Открывающая скобка
      float h = dht.readHumidity();
      float t = dht.readTemperature();

      if (isnan(h) || isnan(t)) {
        MYDEBUG_PRINTLN("Ошибка чтения с DHT22!");
      } else {
        pvT0 = round(t);
        pvT1 = round(h);
        MYDEBUG_PRINT("Влажность: "); MYDEBUG_PRINT(h); MYDEBUG_PRINT(" %\t");
        MYDEBUG_PRINT("Температура: "); MYDEBUG_PRINT(t); MYDEBUG_PRINTLN(" °C");
      }
      break;
    }
    case SENSOR_DS18B20: checkDs18b20(); break;
    case UNKNOWN: MYDEBUG_PRINTLN("Датчики не подключены!"); break;
  }
}

void checkDs18b20(void){
#ifdef DEBUG
  char buff[100];
#endif
  DeviceAddress sensorAddress;        // Переменная для хранения адреса датчика
  for (uint8_t i = 0; i < numberOfDevices; i++){
    float tempC = sensors.getTempCByIndex(i);
    DEBUG_SPRINTF(buff, "TempCByIndex(%i): %5.1f °C",i,tempC);
    MYDEBUG_PRINTLN(buff);
    if(tempC == DEVICE_DISCONNECTED_C) {
      ds[i].errDevice++;
      if(ds[i].errDevice > 5) {ds[i].pvT = 1990; ds[i].errDevice = 5;}
    }
    else {
      ds[i].pvT = tempC * 10;
      ds[i].errDevice = 0;
    }
    //----- Коректировка датчика DS18B20 ---------
    sensors.getAddress(sensorAddress, i);
    uint8_t alarmH = sensors.getHighAlarmTemp(sensorAddress);
    // DEBUG_SPRINTF(buff, "HighAlarmTemp(%i): %3i",i,alarmH);
    // MYDEBUG_PRINTLN(buff);
    if(alarmH == TUNING){
      int8_t alarmL = sensors.getLowAlarmTemp(sensorAddress);
      ds[i].pvT += alarmL;
    }
    if(check_freeze(i)){
      if(i) ERROR2 = 1; else ERROR1 = 1;
    }
  }
  sensors.requestTemperatures();
}

/* int16_t lowPassF2(int16_t PV)
{
float val;
  // val = A1*PVold1-A2*PVold2+A3*PV;
  // PVold2 = PVold1;
  // PVold1 = val;
  return val;
}; */
