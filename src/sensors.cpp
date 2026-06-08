#include "main.h"

#define TUNING	170

/**
 * @brief Detect all connected sensors (DS18B20 AND DHT22) on the same pin.
 */
void sensorType(){
  MYDEBUG_PRINTLN("Detecting sensors...");
  
  // 1. Scan for DS18B20 sensors
  sensors.begin(); // Initialize 1-Wire bus
  numberOfDS18 = sensors.getDS18Count();
  if(numberOfDS18 > 0) {
      if(numberOfDS18 > MAX_DEVICE) numberOfDS18 = MAX_DEVICE;
      MYDEBUG_PRINT("DS18B20 detected: "); MYDEBUG_PRINT(numberOfDS18, DEC); MYDEBUG_PRINTLN(" pcs.");
      sensors.setWaitForConversion(false);
      sensors.setCheckForConversion(false);
      sensors.setAutoSaveScratchPad(false);
      sensors.setResolution(12);
      sensors.requestTemperatures();
      
      for (int i = 0; i < numberOfDS18; i++){
        if(sensors.getAddress(sensorAddresses[i], i)){
          DEBUG_PRINTF("  Sensor %d: ", i);
          printAddress(sensorAddresses[i]);
          MYDEBUG_PRINTLN();
        }
      }
   } else {
      MYDEBUG_PRINTLN("No DS18B20 sensors found.");
   }

   // 2. Try to detect DHT22 regardless of DS18B20 presence
   delay(1000); // Give some time after OneWire scan
   dht.begin();
   float testT = dht.readTemperature();
   if (!isnan(testT)) {
     hasDHT22 = true;
     MYDEBUG_PRINTLN("DHT22 sensor detected.");
   } else {
     hasDHT22 = false;
     MYDEBUG_PRINTLN("DHT22 not found.");
   }
}

/**
 * @brief Main sensor update routine. Handles both DHT22 and DS18B20.
 */
void sensorCheck(){
  // Read DHT22 if present
  if (hasDHT22) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      MYDEBUG_PRINTLN("DHT22 read error!");
      // If DHT fails, we increment error count on ds[0] and ds[1] (primary air control)
      if(++ds[0].errDevice > 5) { ds[0].pvT = 1260; ds[1].pvT = 1260; ds[0].errDevice = 5; }
    } else {
      ds[0].errDevice = 0;
      ds[0].pvT = round(t * 10.0);
      ds[1].pvT = round(h * 10.0);
      MYDEBUG_PRINT("DHT Air t="); MYDEBUG_PRINT(t); MYDEBUG_PRINT(" RH="); MYDEBUG_PRINTLN(h);
    }
  }

  // Read DS18B20 sensors
  if (numberOfDS18 > 0) {
    checkDs18b20();
  }

  // If NO sensors are found at all
  if (!hasDHT22 && numberOfDS18 == 0) {
    MYDEBUG_PRINTLN("ALARM: No sensors connected!");
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
 * If DHT22 is present, DS18B20 values start from ds[2].
 * If DHT22 is NOT present, DS18B20 values start from ds[0] (backward compatibility).
 */
void checkDs18b20(void){
#ifdef DEBUG
  // char buff[100];
#endif
  uint8_t startIdx = hasDHT22 ? 2 : 0;
  
  for (uint8_t i = 0; i < numberOfDS18; i++){
    uint8_t dsIdx = startIdx + i;
    int8_t alarmL = 0;
    // bool calibrated = false;
    if (dsIdx >= MAX_DEVICE) break;

    float tempC = sensors.getTempC(sensorAddresses[i]);
    
    if(tempC == DEVICE_DISCONNECTED_C) {
      if(++ds[dsIdx].errDevice > 5){
        ds[dsIdx].errDevice = 5;
        switch (dsIdx){
        case 0: ERROR1 = 1; break;
        case 1: ERROR2 = 1; break;
        }
      }
    }
    else {
      ds[dsIdx].pvT = round(tempC * 10.0);
      ds[dsIdx].errDevice = 0;
      switch (dsIdx){
      case 0: ERROR1 = 0; break;
      case 1: ERROR2 = 0; break;
      }
    }

    // ---------- Calibration using Alarm registers ---------------
    uint8_t alarmH = sensors.getHighAlarmTemp(sensorAddresses[i]);
    if(alarmH == TUNING){
      alarmL = sensors.getLowAlarmTemp(sensorAddresses[i]);
      ds[dsIdx].pvT += (alarmL);
      // calibrated = true;
    }
    // ---------- Check freeze ------------------------------------
    if(check_freeze(dsIdx, tempC)){
      if(dsIdx == 0) ERROR1 = 1;
      else if(dsIdx == 1) ERROR2 = 1;
    }
    // DEBUG_SPRINTF(buff, "DS18B20[%i] (ds[%u]): %2d.%d °C; ERR=%u", i, dsIdx, ds[dsIdx].pvT / 10, abs(ds[dsIdx].pvT % 10), ds[dsIdx].errDevice);
    // MYDEBUG_PRINT(buff);
    // if(calibrated) {MYDEBUG_PRINT(" Calibrated: "); MYDEBUG_PRINT(alarmL);}
    // MYDEBUG_PRINTLN();
  }
  sensors.requestTemperatures(); // Request for next cycle
}
