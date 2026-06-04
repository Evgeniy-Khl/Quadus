#include "main.h"

#define TUNING	170

/**
 * @brief Detect the type of connected sensor (DS18B20 or DHT22).
 */
void sensorType(){
  MYDEBUG_PRINTLN("Detecting sensor type...");
  // 1. Try to find DS18B20 sensor.
  sensors.begin(); // Initialize 1-Wire bus
  numberOfDevices = sensors.getDS18Count();
  if(numberOfDevices > 0) {
      detectedSensor = SENSOR_DS18B20;
      if(numberOfDevices > MAX_DEVICE) numberOfDevices = MAX_DEVICE;
      MYDEBUG_PRINT("DS18B20 detected: "); MYDEBUG_PRINT(numberOfDevices, DEC); MYDEBUG_PRINTLN(" pcs.");
      sensors.setWaitForConversion(false);    // return control immediately
      sensors.setCheckForConversion(false);
      sensors.setAutoSaveScratchPad(false);
      sensors.setResolution(12);              // Set resolution (9-12 bits)
      sensors.requestTemperatures();          // Command sensors to start conversion
      
      // Get and print addresses for all found sensors
      for (int i = 0; i < numberOfDevices; i++){
        if(sensors.getAddress(sensorAddresses[i], i)){
          DEBUG_PRINTF("  Sensor %d: ", i);
          printAddress(sensorAddresses[i]);
          MYDEBUG_PRINTLN();
        } else {
          DEBUG_PRINTF("Failed to get address for sensor %d\n", i);
        }
      }
   } else {
      // 2. If no DS18B20, try reading DHT22.
      delay(1000);
      dht.begin();
      // Test read. If not NaN, it's a DHT.
      if (!isnan(dht.readTemperature())) {
        detectedSensor = SENSOR_DHT22;
        MYDEBUG_PRINTLN("Sensor detected: DHT22");
      }
   }
}

/**
 * @brief Main sensor update routine.
 */
void sensorCheck(){
  switch (detectedSensor){
    case SENSOR_DHT22:{
      float h = dht.readHumidity();
      float t = dht.readTemperature();

      if (isnan(h) || isnan(t)) {
        MYDEBUG_PRINTLN("DHT22 read error!");
        if(++ds[0].errDevice > 5) {ds[0].pvT = 126; ds[1].pvT = 126; ds[0].errDevice = 5;}
      } else {
        ds[0].errDevice = 0;
        ds[0].pvT = round(t * 10.0);
        ds[1].pvT = round(h * 10.0);
        MYDEBUG_PRINT("t= "); MYDEBUG_PRINT(t); MYDEBUG_PRINTLN(" °C");
        MYDEBUG_PRINT("RH= "); MYDEBUG_PRINT(h); MYDEBUG_PRINT(" %\t");
      }
      break;
    }
    case SENSOR_DS18B20: checkDs18b20(); break;
    case UNKNOWN: MYDEBUG_PRINTLN("No sensors connected!"); break;
  }
}

/**
 * @brief Check if sensor data has frozen (not changing).
 */
bool check_freeze(uint8_t i, float val){
 if(val == ds[i].previousValue){
    if(++ds[i].froze > 600){ds[i].froze = 600; return true;}
 } else {ds[i].froze = 0; ds[i].previousValue = val;}
 return false;
}

/**
 * @brief Process DS18B20 sensor data.
 */
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
    }
    else {
      ds[i].pvT = round(tempC * 10.0);
    }
    // Sensor correction logic using Alarm registers
    uint8_t alarmH = sensors.getHighAlarmTemp(sensorAddresses[i]);
    
    if(alarmH == TUNING){
      int8_t alarmL = sensors.getLowAlarmTemp(sensorAddresses[i]);
      DEBUG_SPRINTF(buff, "High(%i): %3i; Low:%3i",i,alarmH, alarmL);
      MYDEBUG_PRINT(buff);
      ds[i].pvT += (alarmL * 10);
    }
    if(check_freeze(i, tempC)){
      if(i) ERROR2 = 1; else ERROR1 = 1;
    }
    MYDEBUG_PRINTLN();
  }
  MYDEBUG_PRINTLN("--------");
  sensors.requestTemperatures();
}
