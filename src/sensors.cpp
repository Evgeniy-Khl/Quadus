#include "main.h"

#define TUNING	170

void sensorType(){
  MYDEBUG_PRINTLN("Определение типа датчика...");
  // 1. Пытаемся найти датчик DS18B20. Это более надежная проверка.
  sensors.begin(); // Инициализируем шину 1-Wire
  // numberOfDevices = sensors.getDeviceCount();
  numberOfDevices = sensors.getDS18Count();
  if(numberOfDevices > 0) {
      detectedSensor = SENSOR_DS18B20;
      if(numberOfDevices > MAX_DEVICE) numberOfDevices = MAX_DEVICE;
      MYDEBUG_PRINT("Обнаружен датчик DS18B20:"); MYDEBUG_PRINT(numberOfDevices, DEC); MYDEBUG_PRINTLN(" шт.");
      sensors.setWaitForConversion(false);    // false: функция вернет управление немедленно.
      sensors.setCheckForConversion(false);   // Часто используется вместе с waitForConversion = false
      sensors.setAutoSaveScratchPad(false);   // Флаг автоматического сохранения настроек в EEPROM датчика.
      sensors.setResolution(12);// Устанавливаем разрешение для всех датчиков (9, 10, 11, or 12 бит)
      sensors.requestTemperatures(); // Отправляем команду на измерение
      //------- Получаем и сохраняем адреса всех найденных датчиков ------
      for (int i = 0; i < numberOfDevices; i++){
        if(sensors.getAddress(sensorAddresses[i], i)){
          DEBUG_PRINTF("  Датчик %d: ", i);
          printAddress(sensorAddresses[i]);
          MYDEBUG_PRINTLN();
        } else {
          DEBUG_PRINTF("Не удалось получить адрес для датчика %d\n", i);
        }
      }
   } else {
      // 2. Если DS18B20 не найден, пытаемся прочитать данные с DHT22.
      delay(1000);
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
        ds[0].pvT = round(t);
        ds[1].pvT = round(h);
        MYDEBUG_PRINT("t= "); MYDEBUG_PRINT(t); MYDEBUG_PRINTLN(" °C");
        MYDEBUG_PRINT("RH= "); MYDEBUG_PRINT(h); MYDEBUG_PRINT(" %\t");
      }
      break;
    }
    case SENSOR_DS18B20: checkDs18b20(); break;
    case UNKNOWN: MYDEBUG_PRINTLN("Датчики не подключены!"); break;
  }
}

//------------- индикация 66,0 - завис датчик. --------------
bool check_freeze(uint8_t i, float val){
 if(val == ds[i].previousValue){
    if(++ds[i].froze> 600){ds[i].froze = 600; return true;}
 } else {ds[i].froze = 0; ds[i].previousValue = val;}
 return false;
}

void checkDs18b20(void){
#ifdef DEBUG
  char buff[100];
#endif
  for (uint8_t i = 0; i < numberOfDevices; i++){
    float tempC = sensors.getTempC(sensorAddresses[i],3);
    DEBUG_SPRINTF(buff, "tempC(%i): %7.3f °C; ERR=%u; FROZE=%u",i,tempC,ds[i].errDevice,ds[i].froze);
    MYDEBUG_PRINT(buff);
    if(tempC == DEVICE_DISCONNECTED_C) {
      ds[i].errDevice++;
      // if(ds[i].errDevice > 5) {ds[i].pvT = 126; ds[i].errDevice = 5;}
      // DEBUG_SPRINTF(buff, "pvT(%i): %3i °C; err=%u",i,ds[i].pvT,ds[i].errDevice);
      // MYDEBUG_PRINTLN(buff);
    }
    else {
      ds[i].pvT = round(tempC);
      // ds[i].errDevice = 0;
    }
    //----- Коректировка датчика DS18B20 ---------
    uint8_t alarmH = sensors.getHighAlarmTemp(sensorAddresses[i]);
    
    if(alarmH == TUNING){
      int8_t alarmL = sensors.getLowAlarmTemp(sensorAddresses[i]);
      DEBUG_SPRINTF(buff, "High(%i): %3i; Low:%3i",i,alarmH, alarmL);
      MYDEBUG_PRINT(buff);
      ds[i].pvT += alarmL;
    }
    if(check_freeze(i, tempC)){

      if(i) ERROR2 = 1; else ERROR1 = 1;
    }
    MYDEBUG_PRINTLN();
  }
  MYDEBUG_PRINTLN("--------");
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
